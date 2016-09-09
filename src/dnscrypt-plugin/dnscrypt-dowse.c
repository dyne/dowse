/*  Dowse - DNSCrypt proxy plugin for DNS management
 *
 *  (c) Copyright 2016 Dyne.org foundation, Amsterdam
 *  Written by Denis Roio aka jaromil <jaromil@dyne.org>
 *
 * This source code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License as published
 * by the Free Software Foundation; either version 3 of the License,
 * or (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * Please refer to the GNU Public License for more details.
 *
 * You should have received a copy of the GNU Public License along with
 * this source code; if not, write to:
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>

#include <netdb.h>

#include <dnscrypt/plugin.h>
#include <ldns/ldns.h>

#include <hiredis/hiredis.h>
#include <jemalloc/jemalloc.h>

#include <redis.h>
#include <database.h>
#include <epoch.h>

#include "hashmap.h"

DCPLUGIN_MAIN(__FILE__);

// expiration in seconds for the domain hit counter
#define DNS_HIT_EXPIRE 21600

// expiration in seconds for the cache on dns replies
#define DNS_CACHE_EXPIRE 5

#define MAX_QUERY 512
#define MAX_DOMAIN 256
#define MAX_TLD 32
#define MAX_DNS 512  // RFC 6891

#define MAX_LINE 512

typedef struct {
	/////////
	// flags
	int caching;

	////////
	// data

	char query[MAX_QUERY]; // incoming query
	char domain[MAX_DOMAIN]; // domain part parsed (2nd last dot)
	char tld[MAX_TLD]; // tld (domain extension, 1st dot)

	char from[NI_MAXHOST]; // hostname or ip originating the query
	char mac[32]; // mac address (could be just 12 chars)

	// map of known domains
	char *listpath;
	map_t domainlist;

	redisContext *redis;
	redisReply   *reply;

	// using db_runtime to store cached hits
	redisContext *cache;

}  plugin_data_t;

size_t trim(char *out, size_t len, const char *str);
void load_domainlist(plugin_data_t *data);
int free_domainlist_f(any_t arg, any_t element);
char *extract_domain(plugin_data_t *data);
int publish_query(plugin_data_t *data);



const char * dcplugin_description(DCPlugin * const dcplugin) {
	return "Dowse plugin to filter dnscrypt queries";
}


const char * dcplugin_long_description(DCPlugin * const dcplugin) {
	return
		"This plugin checks all settings in Dowse and operates filtering on dnscrypt queries accordingly\n";
}

int dcplugin_init(DCPlugin * const dcplugin, int argc, char *argv[]) {
	int i;
	plugin_data_t *data = malloc(sizeof(plugin_data_t));
	memset(data, 0x0, sizeof(plugin_data_t));

	data->listpath = getenv("DOWSE_DOMAINLIST");
	if(!data->listpath)
		fprintf(stderr,"warning: environmental variable DOWSE_DOMAINLIST not set\n");
	else {
		fprintf(stderr, "Loading domainlist from: %s\n", data->listpath);
		load_domainlist(data);
	}

	data->redis = connect_redis(REDIS_HOST, REDIS_PORT, db_dynamic);

	// TODO: check succesful result

	data->cache = NULL;
	data->caching=0;
	for(i=0; i<argc; i++) {
		fprintf(stderr,"%u arg: %s\n", i, argv[i]);

		if( strncmp(argv[i], "cache", 5) == 0) data->caching=5;
		data->cache = connect_redis(REDIS_HOST, REDIS_PORT, db_runtime);

	}

	dcplugin_set_user_data(dcplugin, data);

	return 0;
}

int
dcplugin_destroy(DCPlugin * const dcplugin)
{

	plugin_data_t *data = dcplugin_get_user_data(dcplugin);

	hashmap_iterate(data->domainlist, free_domainlist_f, NULL);
	hashmap_free(data->domainlist);

	redisFree(data->redis);
	if(data->cache) redisFree(data->cache);

	return 0;
}

DCPluginSyncFilterResult dcplugin_sync_pre_filter(DCPlugin *dcplugin, DCPluginDNSPacket *dcp_packet) {

    struct sockaddr_storage *from_sa;
	socklen_t from_len;
	uint8_t* wire;
	size_t wirelen;

	ldns_pkt *packet = NULL;
	ldns_rr_list *question_rr_list;
	ldns_rr  *question_rr;
	ldns_rdf *question_owner;
	char     *question_str;

	plugin_data_t *data = dcplugin_get_user_data(dcplugin);

	wire = dcplugin_get_wire_data(dcp_packet);
	wirelen = dcplugin_get_wire_data_len(dcp_packet);
	// enforce max dns query size
	if(wirelen > MAX_DNS) return DCP_SYNC_FILTER_RESULT_KILL;

//	fprintf(stderr,"%u bytes received as dns wire request\n", wirelen);

	// import the wire packet to ldns format
	if (ldns_wire2pkt(&packet, wire, wirelen) != LDNS_STATUS_OK)
		return DCP_SYNC_FILTER_RESULT_FATAL;

	// retrieve the actual question string 
	question_rr_list  = ldns_pkt_question(packet);
	question_rr       = ldns_rr_list_rr(question_rr_list, 0U); // first in list)
	question_owner    = ldns_rr_owner(question_rr);
	question_str      = ldns_rdf2str(question_owner);
	// TODO: check if we need to query for more questions here

	if (question_str == NULL)
		// this may be to drastic as FATAL? change if problems occur
		return DCP_SYNC_FILTER_RESULT_FATAL;


	// save the query
	strncpy(data->query, question_str, MAX_QUERY);
	data->query[strlen(data->query)-1] = '\0'; // eliminate terminating dot


	// get the source ip
	from_sa = dcplugin_get_client_address(dcp_packet);
	from_len = dcplugin_get_client_address_len(dcp_packet);
	getnameinfo((struct sockaddr *)from_sa, from_len,
	            data->from, sizeof(data->from), NULL, 0, 0x0);

	// publish info to redis channel
	publish_query(data);

	// resolve locally leased hostnames
	data->reply = redisCommand(data->redis, "GET dns-lease-%s", data->query);
	if(data->reply->len) { // it exists, return that
		fprintf(stderr,"local lease found: %s\n", data->reply->str);
		ldns_pkt *answer_pkt = NULL;
		size_t answer_size = 0;
		ldns_status status;
		uint8_t *outbuf = NULL;
		ldns_rr_list *answer_qr;
		ldns_rr_list *answer_an;
		ldns_rr *answer_an_rr;
		char tmprr[1024];

		// clone the question_rr in answer_qr
		answer_qr = ldns_rr_list_new();
		ldns_rr_list_push_rr(answer_qr, ldns_rr_clone(question_rr));

		// create the answer_an rr_list
		answer_an = ldns_rr_list_new();
		// answer_an_rdf = ldns_rdf_new_frm_str // makes a copy of data buffer
		// 	(LDNS_RDF_TYPE_A, data->reply->str);

		// TODO: optimise by avoiding the string parsing and creating the rdf manually
        //	i.e.	ldns_rr_set_rdf(answer_an_rr, answer_an_rdf, 0U);
		snprintf(tmprr, 1024, "%s 0 IN A %s", data->query, data->reply->str);
		ldns_rr_new_frm_str(&answer_an_rr, tmprr, 0, NULL, NULL); 
		freeReplyObject(data->reply);		// we can free redis here
		

		ldns_rr_list_push_rr(answer_an, answer_an_rr);

		// create the packet and empty fields
		answer_pkt = ldns_pkt_new();

		ldns_pkt_set_qr(answer_pkt, 1);
		ldns_pkt_set_aa(answer_pkt, 1);
		ldns_pkt_set_ad(answer_pkt, 0);

		ldns_pkt_set_id(answer_pkt, ldns_pkt_id(packet));

		ldns_pkt_push_rr_list(answer_pkt, LDNS_SECTION_QUESTION,   answer_qr);
		ldns_pkt_push_rr_list(answer_pkt, LDNS_SECTION_ANSWER,     answer_an);

		// render the packet into wire format (outbuf is saved with a memcpy)
		status = ldns_pkt2wire(&outbuf, answer_pkt, &answer_size);

		// free all the packet structure here, outbuf is the wire format result
		ldns_pkt_free(answer_pkt);

		if (status != LDNS_STATUS_OK) {
			fprintf(stderr,"Error resolving lease: %s\n",
			        ldns_get_errorstr_by_id(status));
			return DCP_SYNC_FILTER_RESULT_FATAL; }

		dcplugin_set_wire_data(dcp_packet, outbuf, answer_size);

		if(outbuf) LDNS_FREE(outbuf);
		return DCP_SYNC_FILTER_RESULT_DIRECT;
	}

	// free the buffers here in any case
	freeReplyObject(data->reply);
	ldns_pkt_free(packet);

//	fprintf(stderr,"%s - query contained this question\n", data->query);

	if(data->cache) {
		// check if the answer is cached (the key is the domain string)
		data->reply = redisCommand(data->cache, "GET dns_cache_%s", data->query);
		if(data->reply->len) { // it exists in cache, return that
//      fprintf(stderr,"%u bytes cache hit!\n", data->reply->len);
			// TODO: a bit dangerous, working directly on the wire packet

			// copy message ID (first 16 bits)
			data->reply->str[0] = wire[0];
			data->reply->str[1] = wire[1];

			dcplugin_set_wire_data(dcp_packet, data->reply->str, data->reply->len);
			freeReplyObject(data->reply);
			return DCP_SYNC_FILTER_RESULT_DIRECT;
		} else
			freeReplyObject(data->reply);
	}

	// if(from_sa->ss_family == AF_PACKET) { // if contains mac address
	// 	char *p = ((struct sockaddr_ll*) from_sa)->sll_addr;
	// 	snprintf(data->mac, 32, "%02x:%02x:%02x:%02x:%02x:%02x",
	// 	         p[0], p[1], p[2], p[3], p[4], p[5]);
	// 	// this is here as a pro-memoria, since we never get AF_P

	// dcplugin_set_user_data(dcplugin, data);

	return DCP_SYNC_FILTER_RESULT_OK;
}


DCPluginSyncFilterResult dcplugin_sync_post_filter(DCPlugin *dcplugin, DCPluginDNSPacket *dcp_packet) {

	plugin_data_t *data;
	uint8_t *wire;
	size_t wirelen;

	data = dcplugin_get_user_data(dcplugin);

	if(data->cache) {
		wire = dcplugin_get_wire_data(dcp_packet);
		wirelen = dcplugin_get_wire_data_len(dcp_packet);

//  fprintf(stderr,"%u bytes reply from dnscrypt received\n", wirelen);


		// check if the query is cached
		data->reply = redisCommand(data->cache, "SET dns_cache_%s %b", data->query, wire, wirelen);
		// TODO: check reply
		freeReplyObject(data->reply);
		data->reply = redisCommand(data->cache, "EXPIRE dns_cache_%s %u", data->query, data->caching); // DNS_HIT_EXPIRE
		freeReplyObject(data->reply);
	}

#if 0
	// test print out ip from packet
	{
		ldns_rr_list *answers;
		ldns_rr      *answer;
		char         *answer_str;
		ldns_rr_type  type;
		size_t        answers_count;
		size_t        i;

		answers = ldns_pkt_answer(packet);
		answers_count = ldns_rr_list_rr_count(answers);
		for (i = (size_t) 0U; i < answers_count; i++) {
			answer = ldns_rr_list_rr(answers, i);
			type = ldns_rr_get_type(answer);
			if (type != LDNS_RR_TYPE_A && type != LDNS_RR_TYPE_AAAA) {
				continue;
			}
			if ((answer_str = ldns_rdf2str(ldns_rr_a_address(answer))) == NULL) {
				return DCP_SYNC_FILTER_RESULT_FATAL;
			}
			fprintf(stderr,"ip: %s\n", answer_str);
			free(answer_str);
		}
	}
#endif

	return DCP_SYNC_FILTER_RESULT_OK;


}



// Stores the trimmed input string into the given output buffer, which must be
// large enough to store the result.  If it is too small, the output is
// truncated.
size_t trim(char *out, size_t len, const char *str) {
	if(len == 0)
		return 0;

	const char *end;
	size_t out_size;

	// Trim leading space
	while(isspace(*str)) str++;

	if(*str == 0)  // All spaces?
	{
		*out = 0;
		return 1;
	}

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace(*end)) end--;
	end++;

	// Set output size to minimum of trimmed string length and buffer size minus 1
	out_size = (end - str) < len-1 ? (end - str) : len-1;

	// Copy trimmed string and add null terminator
	memcpy(out, str, out_size);
	out[out_size] = 0;

	return out_size;
}



int publish_query(plugin_data_t *data) {
	char *extracted;
	int val, res;
	char *sval;

	time_t epoch_t;
	char outnew[MAX_OUTPUT];

	// domain hit count
	extracted = extract_domain(data);
	data->reply = redisCommand(data->redis, "INCR dns_query_%s", extracted);
	val = data->reply->integer;
	freeReplyObject(data->reply);
	data->reply = redisCommand(data->redis, "EXPIRE dns_query_%s %u", extracted, DNS_HIT_EXPIRE); // DNS_HIT_EXPIRE
	freeReplyObject(data->reply);

	// timestamp
	time(&epoch_t);

	// compose the path of the detected query
	snprintf(outnew,MAX_OUTPUT,
	         "DNS,%s,%d,%lu,%s,%s",
	         data->from, val,
	         epoch_t, extracted, data->tld);

	// add domainlist group if found
	if(data->listpath) {
		res = hashmap_get(data->domainlist, extracted, (void**)(&sval));
		if(res==MAP_OK) {// add domain group
			strncat(outnew,",",2);
			strncat(outnew,sval,MAX_OUTPUT-128);
			//snprintf(outnew,MAX_OUTPUT,"%s,%s",outnew,sval);
		}
	}

	data->reply = redisCommand(data->redis, "PUBLISH dns-query-channel %s", outnew);
	freeReplyObject(data->reply);


//	fprintf(stderr,"DNS: %s\n", outnew);

	return 0;
}


void load_domainlist(plugin_data_t *data) {
    char line[MAX_LINE];
    char trimmed[MAX_LINE];
    DIR *listdir = 0;
    struct dirent *dp;
    FILE *fp;

    data->domainlist = hashmap_new();

    // parse all files in directory
    listdir = opendir(data->listpath);
    if(!listdir) {
        perror(data->listpath);
        exit(1); }

    // read file by file
    dp = readdir (listdir);
    while (dp) {
        char fullpath[MAX_LINE];
        snprintf(fullpath,MAX_LINE,"%s/%s",data->listpath,dp->d_name);
        // open and read line by line
        fp = fopen(fullpath,"r");
        if(!fp) {
            perror(fullpath);
            continue; }
        while(fgets(line,MAX_LINE, fp)) {
            // save lines in hashmap with filename as value
            if(line[0]=='#') continue; // skip comments
            trim(trimmed, strlen(line), line);
            if(trimmed[0]=='\0') continue; // skip blank lines
            // logerr("(%u) %s\t%s", trimmed[0], trimmed, dp->d_name);
            hashmap_put(data->domainlist, strdup(trimmed), strdup(dp->d_name));
        }
        fclose(fp);
        dp = readdir (listdir);
    }
    closedir(listdir);
    fprintf(stderr,"size of parsed domain-list: %u\n", hashmap_length(data->domainlist));
}


int free_domainlist_f(any_t arg, any_t element) {
	free(element);
	return MAP_OK;
}


char *extract_domain(plugin_data_t *data) {
    // extracts the last two or three strings of a dotted domain string

    int c;
    int dots = 0;
    int first = 1;
    char *last;
    int positions = 2; // minimum, can become three if sld too short
    int len;
    char address[MAX_QUERY];

    strncpy(address, data->query, MAX_QUERY);
    len = strlen(address);

    /* logerr("extract_domain: %s (%u)",address, len); */

    if(len<3) return(NULL); // a.i

    data->domain[len+1]='\0';
    for(c=len; c>=0; c--) {
        last=address+c;
        if(*last=='.') {
            dots++;
            // take the tld as first dot hits
            if(first) {
                strncpy(data->tld,last,MAX_TLD);
                first=0; }
        }
        if(dots>=positions) {
            char *test = strtok(last+1,".");
            if( strlen(test) > 3 ) break; // its not a short SLD
            else positions++;
        }
        data->domain[c]=*last;
    }

    // logerr("extracted: %s (%p) (dots: %u)", domain+c+1, domain+c+1, dots);

    return(data->domain+c+1);
}
