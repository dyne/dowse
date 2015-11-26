/*  DNSCap plugin for Dowse visualization
 *
 *  (c) Copyright 2015 Dyne.org foundation, Amsterdam
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
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <ctype.h>
#include <errno.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>

#include <time.h>
#include <sys/socket.h>
#include <netdb.h>

#include <dirent.h>

#include "hashmap.h"

#include "../../dnscap_common.h"

static logerr_t *logerr;
/* static int opt_f = 0; */
static const char *filepfx = 0;

static const char *listpath = 0;
static DIR *listdir = 0;
static struct dirent *dp;
static FILE *fp;

static FILE *fileout = 0;

static int console = 1;


#define MAX_QUERY 512
#define MAX_DOMAIN 256
#define MAX_TLD 32
static char query[MAX_QUERY]; // incoming query
static char domain[MAX_DOMAIN]; // domain part parsed (2nd last dot)
static char tld[MAX_TLD]; // tld (domain extension, 1st dot)

static char hostname[MAX_DOMAIN];
char *own_ipv4 = NULL;

output_t dowse_output;

// hash map of visited domains
map_t visited;
// hash map of known domains
map_t domainlist;

void dowse_usage() {
    fprintf(stderr,
            "\ndnscap-dowse.so options:\n"
            "\t-q         don't output anything to console\n"
            "\t-o <arg>   prefix for the output filenames\n"
            "\t-l <arg>   path to domain-list for categories\n"
            "\t-4 <arg>   own IPv4 address\n"
            "\t-i <arg>   network interface\n"
        );
}

void
dowse_getopt(int *argc, char **argv[]) {
    /*
     * The "getopt" function will be called from the parent to
     * process plugin options. */
    int c;
    while ((c = getopt(*argc, *argv, "qo:l:4:i:")) != EOF) {
        switch(c) {
        case 'q':
            console=0;
            break;
        case 'o':
            filepfx = strdup(optarg);
            break;
        case 'l':
            listpath = strdup(optarg);
            break;
        case '4':
            own_ipv4 = strdup(optarg);
            break;
        default:
            dowse_usage();
            exit(0);
        }
    }
}

// Stores the trimmed input string into the given output buffer, which must be
// large enough to store the result.  If it is too small, the output is
// truncated.
size_t trim(char *out, size_t len, const char *str)
{
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
#define MAX_LINE 512

void load_domainlist(const char *path) {
    char line[MAX_LINE];
    char trimmed[MAX_LINE];

    domainlist = hashmap_new();

    // parse all files in directory
    logerr("Parsing domain-list: %s\n", path);
    listdir = opendir(path);
    if(!listdir) {
        perror(path);
        exit(1); }

    // read file by file
    while (dp = readdir (listdir)) {
        char fullpath[MAX_LINE];
        snprintf(fullpath,MAX_LINE,"%s/%s",path,dp->d_name);
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
            hashmap_put(domainlist, strdup(trimmed), strdup(dp->d_name));
        }
        fclose(fp);
    }
    closedir(listdir);
}

int
dowse_start(logerr_t *a_logerr) {
    /*
     * The "start" function is called once, when the program
     * starts.  It is used to initialize the plugin.  If the
     * plugin wants to write debugging and or error messages,
     * it should save the a_logerr pointer passed from the
     * parent code.
     */

    logerr = a_logerr;
    if (filepfx) {
        logerr("Logging to file: %s\n", filepfx);
        fileout = fopen(filepfx, "a");
        if (0 == fileout) {
            logerr("%s: %s\n", filepfx, strerror(errno));
            exit(1);
        }
    }

    // get own hostname
    gethostname(hostname,(size_t)MAX_DOMAIN);

    visited = hashmap_new();

    // load the domain-list path if there
    if(listpath) load_domainlist(listpath);

    return 0;
}

void
dowse_stop() {
    /*
     * The "start" function is called once, when the program
     * is exiting normally.  It might be used to clean up state,
     * free memory, etc.
     */
    int c;
    any_t *e = NULL;

    if(fileout) fclose(fileout);

    if(listdir) hashmap_free(domainlist);

    // TODO free all the elements in map
    /* for(c=MAP_OK; c!=MAP_MISSING; */
    /*     c=hashmap_get_one(visited,e,1)) */
    /*     free(e); */
    // hashmap_get_one is in header but not in implementation

    // free the map
    hashmap_free(visited);
}

int
dowse_open(my_bpftimeval ts) {
    /*
     * The "open" function is called at the start of each
     * collection interval, which might be based on a period
     * of time or a number of packets.  In the original code,
     * this is where we opened an output pcap file.
     */
    return 0;
}

int
dowse_close(my_bpftimeval ts) {
    /*
     * The "close" function is called at the end of each
     * collection interval, which might be based on a period
     * of time or on a number of packets.  In the original code
     * this is where we closed an output pcap file.
     */
    return 0;
}


static inline int is_ip(char *in) {
    // is it a numeric IPv4?
    int numip[4];

    if(sscanf(&query[0],"%u.%u.%u.%u",
              &numip[0],&numip[1],&numip[2],&numip[3]))
        return 4; // ipv4
    else return 0; // not an ip
}

static char *ia_resolv(iaddr ia) {
    int err;

    char *name;
    err = getnameinfo((struct sockaddr*)&ia,sizeof(ia),query,sizeof(query),0,0,0);

    if(err==0) { // all fine, no error

        if( is_ip(query) ) return(query);

        // is it an hostname?
        char *name = strtok(query,".");
        // then remove the domain part
        if(name==NULL) name=&query[0];
        return(name);

    } else { // TODO: here check for errors

        // however we try harder to return the ip
        (void) inet_ntop(ia.af, &ia.u, query, sizeof query);
        return (&query[0]);

    }
    // this never gets here
}

static char *extract_domain(char *address) {
    // extracts the last two or three strings of a dotted domain string

    int c;
    int dots = 0;
    int first = 1;
    char *last;
    int positions = 2; // minimum, can become three if sld too short
    int len = strlen(address);

    /* logerr("extract_domain: %s (%u)",address, len); */

    if(len<3) return(NULL); // a.i

    if(len>MAX_QUERY) {
        logerr("extract_domain: string too long (%s - %u)", address, len);
        return(NULL); }

    domain[len+1]='\0';
    for(c=len; c>=0; c--) {
        last=address+c;
        if(*last=='.') {
            dots++;
            // take the tld as first dot hits
            if(first) {
                strncpy(tld,last,MAX_TLD);
                first=0; }
        }
        if(dots>=positions) {
            char *test = strtok(last+1,".");
            if( strlen(test) > 3 ) break; // its not a short SLD
            else positions++;
        }
        domain[c]=*last;
    }

    // logerr("extracted: %s (%p) (dots: %u)", domain+c+1, domain+c+1, dots);

    return(domain+c+1);
}


#define MAX_OUTPUT 512
void dowse_output(const char *descr, iaddr from, iaddr to, uint8_t proto, int isfrag,
              unsigned sport, unsigned dport, my_bpftimeval ts,
              const u_char *pkt_copy, unsigned olen,
              const u_char *dnspkt, unsigned dnslen) {
    /* dnspkt may be NULL if IP packet does not contain a valid DNS message */

    char output[MAX_OUTPUT];

    if (dnspkt) {

        ns_msg msg;
        int qdcount;
        ns_rr rr;

        int *val;
        char *sval;

        char *extracted;
        char *resolved;
        char *from;
        int res;
        char action = 'A';

        char from_color[16];

        ns_initparse(dnspkt, dnslen, &msg);
        if (!ns_msg_getflag(msg, ns_f_qr)) return;

        /*
         * -- flags --
         * 0    1                5    6    7    8           11               15
         * +----+----------------+----+----+----+-----------+----------------+
         * | QR | Operation Code | AA | TC | RA |   Zero    |    Recode      |
         * +----+----------------+----+----+----+-----------+----------------+
         *
         * Question/Response          : ns_f_qr
         * Operation code             : ns_f_opcode
         * Authoritative Answer       : ns_f_aa
         * Truncation occurred        : ns_f_tc
         * Recursion Desired          : ns_f_rd
         * Recursion Available        : ns_f_ra
         * MBZ                        : ns_f_z
         * Authentic Data (DNSSEC)    : ns_f_ad
         * Checking Disabled (DNSSEC) : ns_f_cd
         * Response code              : ns_f_rcode
         */
        // logerr("msg: %p",msg);
        qdcount = ns_msg_count(msg, ns_s_qd);
        if (qdcount > 0 && 0 == ns_parserr(&msg, ns_s_qd, 0, &rr)) {

            // where the query comes from
            from = ia_resolv(to);

            // if its from ourselves omit it
            if(strncmp(from,hostname,MAX_DOMAIN)==0) return;
            if(own_ipv4) if(strncmp(from,own_ipv4,MAX_DOMAIN)==0) return;
            // not reverse resolved means not known by Dowse, code RED
            if(is_ip(from)) strcpy(from_color,"#FF0000");


            resolved = ns_rr_name(rr);
            // what domain is being looked up
            extracted = extract_domain(resolved);

            res = hashmap_get(visited, extracted, (void**)(&val));
            switch(res) {

            case MAP_MISSING : // never visited
                val = malloc(sizeof(int));
                *val = 1; // just a placeholder for now
                res = hashmap_put(visited, strdup(extracted), val);
                break;

            case MAP_OK: // already visited
                action = 'M';
                break;

                // TODO error checks
            case MAP_FULL:
            case MAP_OMEM:
                break;
            }


            // compose the path of the detected query
            // add category if listed
            if(listpath) { // add known domain list information
                res = hashmap_get(domainlist, extracted, (void**)(&sval));
                switch(res) {

                case MAP_OK:
                    /* render with the category in front of domain */
                    snprintf(output,MAX_OUTPUT,"%lu|%s|%c|%s/%s/%s",
                             ts.tv_sec, from, action, tld, sval, extracted);
                    break;
                default:
                    /* render only the domain in root category */
                    snprintf(output,MAX_OUTPUT,"%lu|%s|%c|%s/%s",
                             ts.tv_sec, from, action, tld, extracted);
                    break;
                }
            } else
                /* render only the domain in root category */
                snprintf(output,MAX_OUTPUT,"%lu|%s|%c|%s/%s",
                         ts.tv_sec, from, action, tld, extracted);



            /* write to file */
            if(fileout) {
                fputs(output, fileout);
                fputc('\n',fileout);
                if(fileout) fflush(fileout);
            }

            /* print fast on console for realtime */
            if(console) {
                puts(output);
                fflush(stdout);
            }

        }


    }

}
