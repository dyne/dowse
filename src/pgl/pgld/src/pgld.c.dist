// pgld - netfilter blocking daemon
//
// Copyright (C) 2009, 2014 Cader <cade.robinson@gmail.com>
// Copyright (C) 2012 hasufell <julian.ospald@googlemail.com>
// Copyright (C) 2011 Dimitris Palyvos-Giannas <jimaras@gmail.com>
// Copyright (C) 2008 Jindrich Makovicka <makovick@gmail.com>
// Copyright (C) 2004 Morpheus <ebutera@users.berlios.de>
//
// This file is part of pgl.
//
// pgl is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// pgl is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with pgl.  If not, see <http://www.gnu.org/licenses/>.


#include "pgld.h"

static unsigned int accept_mark = 0, reject_mark = 0, use_syslog = 0, queue_length = 0, opt_merge = 0, blockfile_count = 0;
static unsigned short queue_num = 0;
static char *pidfile_name = NULL, *logfile_name=NULL, timestr[17];
static FILE *logfile;
static const char *current_charset = 0, **blocklist_filenames = 0, **blocklist_charsets = 0;
static FILE* pidfile = NULL;

#ifdef HAVE_DBUS
static int try_dbus = 0;
// Default is no dbus, enable it with "-d"
static int use_dbus = 0;
//Declarations of dbus functions so that they can be dynamically loaded
static int (*pgl_dbus_init)(void) = NULL;
static void (*pgl_dbus_send)(const char *, va_list) = NULL;
// TODO: static void (*do_dlsym(pgl_dbus_send_ipmatch)(xxxxxxxxxxxxxx) = NULL;
#endif

struct nfq_handle *nfqueue_h = 0;
struct nfq_q_handle *nfqueue_qh = 0;


// General logging function
void do_log(int priority, const char *format, ...) {
    if (use_syslog) {
        va_list ap;
        va_start(ap, format);
        vsyslog(LOG_MAKEPRI(LOG_DAEMON, priority), format, ap);
        va_end(ap);
    }

    if (logfile) {
        va_list ap;
        va_start(ap, format);
        time_t tv;
        struct tm * timeinfo;
        time( &tv );
        timeinfo = localtime ( &tv );
        strftime(timestr, 17, "%b %e %X", timeinfo);
        timestr[16] = '\0';
        fprintf(logfile,"%s ",timestr);
        vfprintf(logfile, format, ap);
        fprintf(logfile, "\n");
        fflush(logfile);
        va_end(ap);
    }

#ifdef HAVE_DBUS
    if (use_dbus) {
        va_list ap;
        va_start(ap, format);
        pgl_dbus_send(format, ap);
        va_end(ap);
    }
#endif

    if (opt_merge) {
        va_list ap;
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        fprintf(stderr,"\n");
        va_end(ap);
    }
}

#ifdef HAVE_DBUS
// Logging function that does not log to dbus
// (to be used e.g. in dbus functions for error messages)
void do_log_xdbus(int priority, const char *format, ...) {

    if (use_syslog) {
        va_list ap;
        va_start(ap, format);
        vsyslog(LOG_MAKEPRI(LOG_DAEMON, priority), format, ap);
        va_end(ap);
    }

    if (logfile) {
        va_list ap;
        va_start(ap, format);
        time_t tv;
        struct tm * timeinfo;
        time( &tv );
        timeinfo = localtime ( &tv );
        strftime(timestr, 17, "%b %e %X", timeinfo);
        timestr[16] = '\0';
        fprintf(logfile,"%s ",timestr);
        vfprintf(logfile, format, ap);
        fprintf(logfile, "\n");
        fflush(logfile);
        va_end(ap);
    }
}
#endif

void int2ip (uint32_t ipint, char *ipstr) {
    ipint=htonl(ipint);
    inet_ntop(AF_INET, &ipint, ipstr, INET_ADDRSTRLEN);
}

#ifdef HAVE_DBUS
static void *dbus_lh = NULL;

#define do_dlsym(symbol)                                                       \
    do {                                                                       \
        dlerror(); /*clear error code*/                                        \
        symbol = dlsym(dbus_lh, # symbol);                                     \
        err = dlerror();                                                       \
        if (err) {                                                             \
            do_log_xdbus(LOG_ERR, "ERROR: Cannot get symbol %s: %s", # symbol, err); \
            goto out_err;                                                      \
        }                                                                      \
    } while (0)

static int open_dbus() {

    char *err;

    dbus_lh = dlopen(PLUGINDIR "/libdbus.so", RTLD_NOW);
    if (!dbus_lh) {
        do_log(LOG_ERR, "ERROR: dlopen() failed: %s", dlerror());
        return -1;
    }
    dlerror(); // clear the error flag

    do_dlsym(pgl_dbus_init);
    do_dlsym(pgl_dbus_send);
//  do_dlsym(pgl_dbus_send_ipmatch);
    return 0;

out_err:
    dlclose(dbus_lh);
    dbus_lh = 0;
    return -1;
}

static int close_dbus() {
    int ret = 0;


    use_dbus = 0;

    if (dbus_lh) {
        ret = dlclose(dbus_lh);
        dbus_lh = 0;
    }

    return ret;
}

#endif  /*HAVE_DBUS*/

static FILE *create_pidfile(const char *name) {
    FILE *f;

    f = fopen(name, "w");
    if (f == NULL) {
        do_log(LOG_ERR, "Unable to create PID file %s: %s\n", name, strerror(errno));
        return NULL;
    }

    /* this works even if pidfile is stale after daemon is sigkilled */
    if (lockf(fileno(f), F_TLOCK, 0) == -1) {
        do_log(LOG_ERR, "Unable to set exclusive lock for pidfile %s: %s\n", name, strerror(errno));
        return NULL;
    }

    fprintf(f, "%d\n", getpid());
    fflush(f);

    /* leave fd open as long as daemon is running */
    /* this is useful for example so that inotify can catch a file
     * closed event even if daemon is killed */
    return f;
}

// Once daemonized stdout and stderr are no more available, only logging to
// syslog, logfile and dbus.
static void daemonize() {
    /* Fork off and have parent exit. */
    switch (fork()) {
    case -1:
        perror("fork");
        exit(1);
    case 0:
        break;
    default:
        exit(0);
    }

    /* Detach from the controlling terminal */
    setsid();

    /* Close all standard I/0 descriptors */
    close(fileno(stdin));
    close(fileno(stdout));
    close(fileno(stderr));

    do_log(LOG_INFO, "INFO: Started.");
}

static int load_all_lists() {
    int i, ret = 0;

    blocklist_clear(0);
    if (blockfile_count) {
        for (i = 0; i < blockfile_count; i++) {
            if (load_list(blocklist_filenames[i], blocklist_charsets[i])) {
                do_log(LOG_ERR, "ERROR: Error loading %s", blocklist_filenames[i]);
                ret = -1;
            }
        }
    } else {
        //assume stdin for list
        load_list(NULL,NULL);
    }
    blocklist_sort();
    blocklist_merge();
    do_log(LOG_INFO, "INFO: Blocking %u IP ranges (%u IPs).", blocklist.count, blocklist.numips);
    return ret;
}

static void nfqueue_unbind() {
    if (!nfqueue_h)
        return;
    do_log(LOG_INFO, "INFO: Unbinding from nfqueue: %hu", queue_num);
    nfq_destroy_queue(nfqueue_qh);
    if (nfq_unbind_pf(nfqueue_h, AF_INET) < 0) {
        do_log(LOG_ERR, "ERROR: Error during nfq_unbind_pf(): %s", strerror(errno));
    }
    nfq_close(nfqueue_h);
}

static void sighandler(int sig, siginfo_t *info, void *context) {
    switch (sig) {
    case SIGUSR1:
        // dump and reset stats
        blocklist_stats(1);
        break;
    case SIGUSR2:
        // just dump stats
        blocklist_stats(0);
        break;
    case SIGHUP:
        if (logfile_name != NULL) {
            do_log(LOG_INFO, "INFO: Closing logfile: %s", logfile_name);
            fclose(logfile);
            logfile=NULL;
            if ((logfile=fopen(logfile_name,"a")) == NULL) {
                do_log(LOG_ERR, "ERROR: Unable to open logfile: %s", logfile_name);
                perror(" ");
                exit(-1);
            } else {
                do_log(LOG_INFO, "INFO: Reopened logfile: %s", logfile_name);
            }
        }
        if (load_all_lists() < 0) {
            do_log(LOG_ERR, "ERROR: Cannot reload the blocklist(s).");
        }
        do_log(LOG_INFO, "INFO: Blocklist(s) reloaded.");
        break;
    case SIGTERM:
    case SIGINT:
        nfqueue_unbind();
        blocklist_stats(0);
#ifdef HAVE_DBUS
        if (use_dbus)
            close_dbus();
#endif
        blocklist_clear(0);
        free(blocklist_filenames);
        free(blocklist_charsets);
        // close syslog
        if (use_syslog) {
            closelog();
        }
        if (pidfile) {
            fclose(pidfile);
            unlink(pidfile_name);
        }
        exit(0);
        break;
    case SIGSEGV:
        nfqueue_unbind();
        abort();
        break;
    default:
        break;
    }
}

static int install_sighandler() {
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = sighandler;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;

    if (sigaction(SIGUSR1, &sa, NULL) < 0) {
        perror("Error setting signal handler for SIGUSR1\n");
        return -1;
    }
    if (sigaction(SIGUSR2, &sa, NULL) < 0) {
        perror("Error setting signal handler for SIGUSR2\n");
        return -1;
    }
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        perror("Error setting signal handler for SIGHUP\n");
        return -1;
    }
    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        perror("Error setting signal handler for SIGTERM\n");
        return -1;
    }
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("Error setting signal handler for SIGINT\n");
        return -1;
    }
    if (sigaction(SIGSEGV, &sa, NULL) < 0) {
        perror("Error setting signal handler for SIGABRT\n");
        return -1;
    }
    return 0;
}

static void setipinfo (char *src, char *dst, char *proto, struct iphdr *ip, char *payload){
    struct udphdr *udp;
    struct tcphdr *tcp;
    switch (ip->protocol) {
    case TCP:
        strcpy(proto, "TCP");
        tcp     = (struct tcphdr*) (payload + (4 * ip->ihl));
        sprintf(src, "%u.%u.%u.%u:%u",NIPQUAD(ip->saddr),ntohs(tcp->source));
        sprintf(dst, "%u.%u.%u.%u:%u",NIPQUAD(ip->daddr),ntohs(tcp->dest));
        break;
    case UDP:
        strcpy(proto, "UDP");
        udp     = (struct udphdr*) (payload + (4 * ip->ihl));
        sprintf(src, "%u.%u.%u.%u:%u",NIPQUAD(ip->saddr),ntohs(udp->source));
        sprintf(dst, "%u.%u.%u.%u:%u",NIPQUAD(ip->daddr),ntohs(udp->dest));
        break;
    case ICMP:
        strcpy(proto, "ICMP");\
        sprintf(src, "%u.%u.%u.%u",NIPQUAD(ip->saddr));
        sprintf(dst, "%u.%u.%u.%u",NIPQUAD(ip->daddr));
        break;
    default:
        sprintf(proto, "%d", ip->protocol);
        sprintf(src, "%u.%u.%u.%u",NIPQUAD(ip->saddr));
        sprintf(dst, "%u.%u.%u.%u",NIPQUAD(ip->daddr));
        break;
    }
}

static int nfqueue_cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa, void *data) {
    int id = 0, status = 0;
    struct nfqnl_msg_packet_hdr *ph;
    block_entry_t *found_range;
    struct iphdr *ip;
    unsigned char *payload;
    char proto[5], src[23], dst[23];  //src and dst are 23 for IP(16)+port(5) + : + NULL

    ph = nfq_get_msg_packet_hdr(nfa);
    if (ph) {
        id = ntohl(ph->packet_id);
        nfq_get_payload(nfa, &payload);
        ip = (struct iphdr*) payload;
        switch (ph->hook) {
        case NF_IP_LOCAL_IN:
            found_range = blocklist_find(ntohl(ip->saddr));
            if (found_range) {
                // we set the user-defined reject_mark and set NF_REPEAT verdict
                // it's up to other iptables rules to decide what to do with this marked packet
                nfq_set_verdict2(qh, id, NF_REPEAT, reject_mark, 0, NULL);
                found_range->hits++;
                // NOTE: "setipinfo" sets the formats.
                // TODO: Separate IP and port in there.
                setipinfo(src, dst, proto, ip, payload);
#ifndef LOWMEM
                do_log(LOG_NOTICE, " IN: %-22s %-22s %-4s || %s",src,dst,proto,found_range->name);
#else
                do_log(LOG_NOTICE, " IN: %-22s %-22s %-4s",src,dst,proto);
#endif
            } else {
                // we set the user-defined accept_mark and set NF_REPEAT verdict
                // it's up to other iptables rules to decide what to do with this marked packet
                nfq_set_verdict2(qh, id, NF_REPEAT, accept_mark, 0, NULL);
            }
            break;
        case NF_IP_LOCAL_OUT:

            found_range = blocklist_find(ntohl(ip->daddr));
            if (found_range) {
                // we set the user-defined reject_mark and set NF_REPEAT verdict
                // it's up to other iptables rules to decide what to do with this marked packet
                nfq_set_verdict2(qh, id, NF_REPEAT, reject_mark, 0, NULL);
                found_range->hits++;
                // NOTE: "setipinfo" sets the formats.
                // TODO: Separate IP and port in there.
                setipinfo(src, dst, proto, ip, payload);
#ifndef LOWMEM
//                 TODO: (here and in IN and FWD)
// #ifdef HAVE_DBUS
//                 do_log_xdbus(LOG_NOTICE, " IN: %-16s %-5s %-16s %-5s %-4s %s",src,srcport,dst,dstport,proto,found_range->name);
//                 if (use_dbus) {
//                     va_list ap;
//                     va_start(ap, format);
//                   //pgl_dbus_send(format, ap);
//                     pgl_dbus_send_ipmatch("IN: %-16s %-5s %-16s %-5s %-4s %s",src,srcport,dst,dstport,proto,found_range->name);
//                     va_end(ap);
//                 }
// #else
                do_log(LOG_NOTICE, "OUT: %-22s %-22s %-4s || %s",src,dst,proto,found_range->name);
// #endif
#else
                do_log(LOG_NOTICE, "OUT: %-22s %-22s %-4su", src,dst,proto);
#endif
            } else {
                // we set the user-defined accept_mark and set NF_REPEAT verdict
                // it's up to other iptables rules to decide what to do with this marked packet
                nfq_set_verdict2(qh, id, NF_REPEAT, accept_mark, 0, NULL);
            }
            break;
        case NF_IP_FORWARD:
            found_range = blocklist_find(ntohl(ip->saddr));
            if (!found_range) {
                found_range = blocklist_find(ntohl(ip->daddr));
            }
            if (found_range) {
                // we set the user-defined reject_mark and set NF_REPEAT verdict
                // it's up to other iptables rules to decide what to do with this marked packet
                nfq_set_verdict2(qh, id, NF_REPEAT, reject_mark, 0, NULL);
                found_range->hits++;
                setipinfo(src, dst, proto, ip, payload);
#ifndef LOWMEM
                do_log(LOG_NOTICE, "FWD: %-22s %-22s %-4s || %s",src,dst,proto,found_range->name);
#else
                do_log(LOG_NOTICE, "FWD: %-22s %-22s %-4s",src,dst,proto);
#endif
            } else {
                // we set the user-defined accept_mark and set NF_REPEAT verdict
                // it's up to other iptables rules to decide what to do with this marked packet
                nfq_set_verdict2(qh, id, NF_REPEAT, accept_mark, 0, NULL);
            }
            break;
        default:
            do_log(LOG_NOTICE, "WARN: Not NF_LOCAL_IN/OUT/FORWARD packet!");
            break;
        }
    } else {
        do_log(LOG_ERR, "ERROR: NFQUEUE: can't get msg packet header.");
        return 1;               // from nfqueue source: 0 = ok, >0 = soft error, <0 hard error
    }
    return 0;
}

static int nfqueue_bind() {
    nfqueue_h = nfq_open();
    if (!nfqueue_h) {
        do_log(LOG_ERR, "ERROR: Error during nfq_open(): %s", strerror(errno));
        return -1;
    }

    if (nfq_unbind_pf(nfqueue_h, AF_INET) < 0) {
        do_log(LOG_ERR, "ERROR: Error during nfq_unbind_pf(): %s", strerror(errno));
        nfq_close(nfqueue_h);
        return -1;
    }

    if (nfq_bind_pf(nfqueue_h, AF_INET) < 0) {
        do_log(LOG_ERR, "ERROR: Error during nfq_bind_pf(): %s", strerror(errno));
        nfq_close(nfqueue_h);
        return -1;
    }

    do_log(LOG_INFO, "INFO: Binding to queue %hu", queue_num);
    if (accept_mark) {
        do_log(LOG_INFO, "INFO: ACCEPT mark: %u", accept_mark);
    }
    if (reject_mark) {
        do_log(LOG_INFO, "INFO: REJECT mark: %u", reject_mark);
    }
    nfqueue_qh = nfq_create_queue(nfqueue_h, queue_num, &nfqueue_cb, NULL);
    if (!nfqueue_qh) {
        do_log(LOG_ERR, "ERROR: Error during nfq_create_queue(): %s", strerror(errno));
        nfq_close(nfqueue_h);
        return -1;
    }

    if (nfq_set_mode(nfqueue_qh, NFQNL_COPY_PACKET, PAYLOADSIZE) < 0) {
        do_log(LOG_ERR, "ERROR: Can't set packet_copy mode: %s", strerror(errno));
        nfq_destroy_queue(nfqueue_qh);
        nfq_close(nfqueue_h);
        return -1;
    }

    if ( queue_length > 0) {
        if (nfq_set_queue_maxlen(nfqueue_qh, queue_length) < 0) {
            do_log(LOG_ERR, "ERROR: Can't set queue max length: %s", strerror(errno));
            do_log(LOG_INFO, "INFO: Continuing anyway with default queue max length.");
        } else {
            do_log(LOG_INFO, "INFO: Set netfilter queue length to %u packets", queue_length);
        }
    }
    return 0;
}

static void nfqueue_loop () {
    struct nfnl_handle *nh;
    int fd, rv;
    char buf[RECVBUFFSIZE];
//  struct pollfd fds[1];

    if (nfqueue_bind() < 0) {
        do_log(LOG_ERR, "ERROR: Error binding to queue: %hu", queue_num);
        exit(1);
    }

    nh = nfq_nfnlh(nfqueue_h);
    fd = nfnl_fd(nh);

    while ((rv = recv(fd, buf, sizeof(buf), 0)) >= 0) {
        nfq_handle_packet(nfqueue_h, buf, rv);
    }
    int err=errno;
    if ( err == ENOBUFS ) {
        do_log(LOG_ERR, "ERROR: ENOBUFS error on queue '%hu'. Use pgld -Q option or set in pglcmd.conf NFQUEUE_MAXLEN to increase buffers, recv returned %s", queue_num, strerror(err));
        /* close and return, nfq_destroy_queue() won't work as we've no buffers */
        nfq_close(nfqueue_h);
        exit(1);
    } else {
        do_log(LOG_ERR, "ERROR: Error on queue '%hu', recv returned %s", queue_num, strerror(err));
        nfqueue_unbind();
        exit(0);
    }
}

static void print_usage() {
    fprintf(stderr, PACKAGE_NAME " " VERSION "\n\n");
    fprintf(stderr, "pgl is licensed under the GNU General Public License v3, or (at your option)\n");
    fprintf(stderr, "any later version. This program comes with ABSOLUTELY NO WARRANTY. This is\n");
    fprintf(stderr, "free software, and you are welcome to modify and/or redistribute it.\n\n");
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  pgld [-s] [-l LOGFILE]");
#ifdef HAVE_DBUS
    fprintf(stderr, " [-d]");
#endif
    fprintf(stderr, " [-c CHARSET] [-p PIDFILE] -a MARK -r MARK [-q 0-65535] [-Q queue_size] BLOCKLIST(S)\n");
    fprintf(stderr, "  pgld [-c CHARSET] -m [BLOCKLIST(S)]\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -s                Enable logging to syslog.\n");
    fprintf(stderr, "  -l LOGFILE        Enable logging to LOGFILE.\n");
#ifdef HAVE_DBUS
    fprintf(stderr, "  -d                Enable D-Bus support.\n");
#endif
    fprintf(stderr, "  -c CHARSET        Specify blocklist file CHARSET.\n");
    fprintf(stderr, "  -p PIDFILE        Use a PIDFILE.\n");
    fprintf(stderr, "  -q 0-65535        Specify a 16-bit NFQUEUE number.\n");
    fprintf(stderr, "                    Must match --queue-num used in iptables rules.\n");
    fprintf(stderr, "  -Q queue_size     Specify a queue length in packets. If not specified use default.\n");
    fprintf(stderr, "  -r MARK           Place a 32-bit MARK on rejected packets.\n");
    fprintf(stderr, "  -a MARK           Place a 32-bit MARK on accepted packets.\n");
    fprintf(stderr, "  -m [BLOCKLIST(S)] Load, sort, merge, and dump blocklist(s) to stdout.\n");
    fprintf(stderr, "                    Specify blocklists or pipe from stdin.\n");
    fprintf(stderr, "  -h                Print this help.\n");
}

void add_blocklist(const char *name, const char *charset) {
    blocklist_filenames = (const char**)realloc(blocklist_filenames, sizeof(const char*) * (blockfile_count + 1));
    CHECK_OOM(blocklist_filenames);
    blocklist_charsets = (const char**)realloc(blocklist_charsets, sizeof(const char*) * (blockfile_count + 1));
    CHECK_OOM(blocklist_charsets);
    blocklist_filenames[blockfile_count] = name;
    blocklist_charsets[blockfile_count] = charset;
    blockfile_count++;
}

int main(int argc, char *argv[]) {
    int opt, i;

    while ((opt = getopt(argc, argv, "sl:"
#ifdef HAVE_DBUS
        "d"
#endif
        "c:p:q:Q:r:a:mh" )) != -1) {
        switch (opt) {
        case 's':
            use_syslog = 1;
            break;
        case 'l':
            logfile_name=malloc(strlen(optarg)+1);
            CHECK_OOM(logfile_name);
            strcpy(logfile_name,optarg);
            break;
#ifdef HAVE_DBUS
        case 'd':
            try_dbus = 1;
            break;
#endif
        case 'c':
            current_charset = optarg;
            break;
        case 'p':
            pidfile_name=malloc(strlen(optarg)+1);
            CHECK_OOM(pidfile_name);
            strcpy(pidfile_name,optarg);
            break;
        case 'q':
            queue_num = (uint16_t)atoi(optarg);
            break;
        case 'Q':
            queue_length = (uint32_t)atoi(optarg);
            break;
        case 'r':
            reject_mark = (uint32_t)atoi(optarg);
            break;
        case 'a':
            accept_mark = (uint32_t)atoi(optarg);
            break;
        case 'm':
            opt_merge = 1;
            break;
        case 'h':
            print_usage();
            exit(0);
        }
    }

    for (i = 0; i < argc - optind; i++) {
        add_blocklist(argv[optind + i], current_charset);
    }

    // Do merge and exit - for just merging lists and not daemonizing.
    if (opt_merge) {
        blocklist_init();
        load_all_lists();
        blocklist_dump();
        exit(0);
    }

    if (blockfile_count == 0) {
        fprintf(stderr, "\nERROR: No blocklist specified!\n\n");
        print_usage();
        exit(1);
    }

    if (queue_num < 0 || queue_num > 65535) {
        fprintf(stderr, "\nERROR: Invalid queue number! Must be 0-65535\n\n");
        print_usage();
        exit(1);
    }

    if (pidfile_name == NULL) {
        pidfile_name=malloc(strlen(PIDFILE)+1);
        CHECK_OOM(pidfile_name);
        strcpy(pidfile_name,PIDFILE);
    }

    // open logfile
    if (logfile_name != NULL) {
        if ((logfile=fopen(logfile_name,"a")) == NULL) {
            fprintf(stderr, "Unable to open logfile: %s", logfile_name);
            print_usage();
            perror(" ");
            exit(-1);
        }
    }

    // open syslog
    if (use_syslog) {
        openlog("pgld", 0, LOG_DAEMON);
    }

    // connect to dbus
#ifdef HAVE_DBUS
    if (try_dbus) {
        if (open_dbus() < 0) {
            do_log(LOG_ERR, "ERROR: Cannot load D-Bus plugin");
            try_dbus = 0;
            exit(1);
        }
        if (pgl_dbus_init() < 0) {
            do_log(LOG_ERR, "ERROR: Cannot initialize D-Bus");
            try_dbus = 0;
            exit(1);
        }
    }
    use_dbus = try_dbus;
#endif

    daemonize();

    // pidfile has to be created *after* daemonize (results in a new pid)
    pidfile = create_pidfile(pidfile_name);
    if (!pidfile) {
        do_log(LOG_ERR, "ERROR: Error creating pidfile %s", pidfile_name);
        exit(1);
    }

    if (install_sighandler() != 0) {
        do_log(LOG_ERR, "ERROR: Error installing signal handlers.");
        exit(1);
    }

    blocklist_init();
    if (load_all_lists() < 0) {
        do_log(LOG_ERR, "ERROR: Cannot load the blocklist(s)");
        return -1;
    }

    nfqueue_loop();
}
