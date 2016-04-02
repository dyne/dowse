/*  sup 1.1
 *
 *  (c) 2016 Dyne.org Foundation, Amsterdam
 *
 *  Written by:
 *  2009-2011 pancake <nopcode.org>
 *  2016      Denis Roio <jaromil@dyne.org>
 *
 * This source code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  Please refer
 * to the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this source code; if not, write to: Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <libgen.h>
#include <pwd.h>

struct rule_t {
    int uid;
    int gid;
    const char *cmd;
    const char *path;
    const char *hash;
};

#include "config.h"

#ifdef HASH
#include "sha256.h"
#endif

static const char *HEADER = "sup " VERSION " - small and beautiful superuser tool\n";

static const char *COPYLEFT =
    "copyright (C) 2016 dyne.org foundation, license GNU GPL v3+\n"
    "this is free software: you are free to change and redistribute it\n"
    "for the latest sourcecode go to <https://git.devuan.org/jaromil/sup>\n";

static const char *LICENSE =
    "this source code is distributed in the hope that it will be useful,\n"
    "but without any warranty; without even the implied warranty of\n"
    "merchantability or fitness for a particular purpose.\n"
    "when in need please refer to <http://dyne.org/support>.\n";


static const char *HELP =
    "Syntax: sup [options] command [arguments...]\n"
    "\n"
    "Options:\n"
    " -l     list compiled-in authorizations and flags\n"
    " -u     set uid to this user name\n"
    " -g     set gid to this group name\n"
    " -d     fork command as background process (daemon)\n"
    " -p     saves pid of background process to file (daemon)\n"
    "\n"
    "Please report bugs to <https://git.devuan.org/jaromil/sup/issues>\n";

#define MAXCMD 512
#define MAXFILEPATH 4096
#define MAXBINSIZE 10485760 // 10 MiBs

/* Always return 1 on error, conforming to standard shell checks.
   Reason of error is described by stderr text before colon,
   extended reason can be provided or falls back to errno. */
static int error(const char *code, const char *reason) {
    fprintf (stderr, "%s: %s\n",
             code? code : "",
             reason? reason : strerror (errno));
    exit(1);
}

static char *getpath(const char *str) {
    struct stat st;
    static char file[MAXFILEPATH];
    char *p, *path = getenv ("PATH");
    if (path)
        for (p = path; *p; p++) {
            if (*p==':' && (p>path&&*(p-1)!='\\')) {
                *p = 0;
                snprintf (file, sizeof (file)-1, "%s/%s", path, str);
                if (!lstat (file, &st))
                    return file;
                *p = ':';
                path = p+1;
            }
        }
    return NULL;
}

#ifdef HASH

#define CHUNK 1048576 // 1MiB
static uint32 getsha(const char *path, unsigned char *dest) {
    static sha256_context sha;

    unsigned char buf[CHUNK]; // 1 MiB
    uint32 len, tot;
    FILE *fd;

    fd = fopen(path,"r");
    if(!fd) error("fopen", "cannot read binary file");

    sha256_starts(&sha);
    clearerr(fd);
    len=0;
    tot=0;

    do {
        // read chunk of data in binary file
        len = fread(buf, 1, CHUNK, fd);
        if(!len) break; // NULL read
        tot+=len;
        if(len<CHUNK) break; // EOF reached

        // compute sha256 of chunk
        sha256_update(&sha, buf, len);

    } while(len>0);

    // check file descriptor for errors
    if(ferror(fd)) {
        fclose(fd);
        error("fread", "error reading binary file");
    }
    fclose(fd);

    // compute last chunk
    if(len>0) sha256_update(&sha, buf, len);

    // finish and save result in *dest
    sha256_finish(&sha, dest);

    return(tot);
}

#endif


int main(int argc, char **argv) {

    static char fullcmd[MAXCMD];
    static char *cmd;

    struct passwd *pw;
    struct stat st;

    static int i, uid, gid;
    static int target_uid=0;
    static int target_gid=0;

#ifdef HASH
    unsigned char digest[32];
    char output[65];
#endif

#ifdef DAEMON
    int fork_daemon = 0;
    char pidfile[MAXFILEPATH] = "";
#endif

    // parse commandline options
    int opt;
    while((opt = getopt(argc, argv, "+hvdlu:g:p:")) != -1) {

        switch(opt) {

#ifdef DAEMON
        case 'p':
            snprintf(pidfile,MAXFILEPATH,"%s",optarg);
            break;
#endif

        case 'u':
            {
                struct passwd *puid;
                errno=0;
                puid=getpwnam(optarg);
                if(!puid && errno) error("uid_getpwnam",NULL);
                if(puid) target_uid=puid->pw_uid;
            }
            break;

        case 'g':
            {
                struct passwd *pgid;
                errno=0;
                pgid=getpwnam(optarg);
                if(!pgid && errno) error("gid_getpwnam",NULL);
                if(pgid) target_gid=pgid->pw_gid;
            }
            break;

        case 'h':
            fprintf(stdout, "%s\n%s\n%s", HEADER, COPYLEFT, HELP);
            exit (0);

        case 'v':
            fprintf(stdout, "%s\n%s\n%s", HEADER, COPYLEFT, LICENSE);

            exit (0);

#ifdef DAEMON
        case 'd':
            fork_daemon=1;
            break;
#endif

        case 'l':
            fprintf(stdout,"%s\n%s\nList of compiled in authorizations:\n\n", HEADER, COPYLEFT);
            fprintf(stdout,"User\tUID\tGID\t%s\t\t%s\n",
                    "Command","Forced PATH");
            for (i = 0; rules[i].cmd != NULL; i++) {
                /* Using 'getpwuid' in statically linked applications
                   requires at runtime the shared libraries from the glibc
                   version used for linking. But not in case of musl-libc. */
                pw = getpwuid( rules[i].uid );
                fprintf (stdout, "%s\t%d\t%d\t%s\t%s\n",
                         pw?pw->pw_name:"", rules[i].uid, rules[i].gid,
                         rules[i].cmd, rules[i].path);
#ifdef HASH
                fprintf(stdout, "sha256: %s\n\n",rules[i].hash);
#endif
            }
            fprintf(stdout,"\nFlags: %s %s %s %s\n",
#ifdef HASH
                    HASH?"HASH":"",
#else
                    "",
#endif
#ifdef DAEMON
                    DAEMON?"DAEMON":"",
#else
                    "",
#endif
                    strlen(CHROOT)?"CHROOT":"",
                    strlen(CHRDIR)?"CHRDIR":"");
            exit (0);

        } // switch(opt)

    } // getopt

    // get the called UID and GID
    uid = getuid ();
    gid = getgid ();

    // copy the execv argument locally
    snprintf(fullcmd,MAXCMD,"%s",argv[optind]);
    // save a pointer to basename string in cmd
    cmd = basename(fullcmd);

    // get the username string from /etc/passwd
    pw = getpwuid( uid );
#ifdef DEBUG
    /* one could maintain a log of calls here */
    fprintf(stderr,"sup %s called by %s(%d) gid(%d)\n",
            cmd, pw?pw->pw_name:"", uid, gid);
#endif

    // loop over each rule
    for (i = 0; rules[i].cmd != NULL; i++) {

        /// COMMAND AND PATH CHECK
        // if command is * or matching the rule
        if (*rules[i].cmd == '*' || !strcmp (cmd, rules[i].cmd)) {
            // if path is locked
            if (*rules[i].path != '*') {
                // and if path is specified
                if((fullcmd[0]=='.')||(fullcmd[0]=='/')) {
                    // then check that path matches
                    if( strcmp(rules[i].path,fullcmd) )
                        return error("path","path not matching");
                // or if path is not specified
                } else { // get the default path with our getpath()
                    snprintf(fullcmd,MAXCMD,"%s",getpath(cmd));
                    // check if the default environment path matches
                    if( strcmp(rules[i].path,fullcmd) )
                        return error("path","path not matching");
                }
            // or if path is not locked
            } else // and if path is not specified, getpath()
                if((fullcmd[0]!='.')&&(fullcmd[0]!='/'))
                    snprintf(fullcmd,MAXCMD,"%s",getpath(cmd));

#ifdef DEBUG
            fprintf(stderr,"path check passed\n");
            fprintf(stderr,"fullcmd: %s\n",fullcmd);
            fprintf(stderr,"cmd: %s\n",cmd);
#endif

            /// COMMAND BINARY CHECK
            // command does not exist as binary on the filesystem
            if (lstat (fullcmd, &st) == -1)
                return error("lstat", "cannot stat program");
            // command has wrong permissions (writable to others)
            if (st.st_mode & 0022)
                return error("perm", "cannot run binaries others can write.");
            // user UID is not root
            if (uid != SETUID
                // and is not unlocked
                && rules[i].uid != -1
                // and is not the locked UID
                && rules[i].uid != uid)
                return error("uid", "user does not match");

            // user GID is not root
            if (gid != SETGID
                // and is not unlocked
                && rules[i].gid != -1
                // and is not the locked GID
                && rules[i].gid != gid)
                return error("gid", "group id does not match");


#ifdef HASH
            /// BINARY HASH CHECKSUM
            if( strlen(rules[i].hash) ) {
                int c;
                uint32 sizeread;

                if(st.st_size>MAXBINSIZE)
                    error("binsize", "cannot check hash of file, size too large");

                sizeread = getsha(fullcmd, digest);
                if(sizeread != st.st_size)
                    error("getsha", "binary file size differs from size read");

                for(c = 0; c<32; c++)
                    sprintf(output + (c * 2),"%02x",digest[c]);
                output[64] = '\0';

                if(strncmp(rules[i].hash, output, 64)!=0) {
                    fprintf(stderr,"%s\n%s\n", rules[i].hash, output);
                    return error("hash", "hash does not match");
                }
            }
#endif

            // privilege escalation done here
            if (setuid (target_uid) <0)
                return error("setuid",NULL);
            if (setgid (target_gid) <0)
                return error("setgid",NULL);
            if (seteuid (target_uid) <0)
                return error("seteuid",NULL);
            if (setegid (target_gid) <0)
                return error("setegid",NULL);

#ifdef CHROOT
            if (*CHROOT && (target_uid==0))
                if (chdir (CHROOT) == -1 || chroot (".") == -1)
                    return error("chroot", NULL);
            if (*CHRDIR)
                if (chdir (CHRDIR) == -1)
                    return error("chdir", NULL);
#endif

#ifdef DAEMON
            if(fork_daemon) {

                pid_t pid;
                pid = fork();
                if(pid<0) return error("fork", NULL);

                else if(pid==0) { // child

                    int fd = open("/dev/tty", O_RDWR);
                    ioctl(fd, TIOCNOTTY, 0);
                    close(fd);
                    chdir("/");
                    umask(022); // secure default
                    setpgid(0,0);  // process group
                    fd=open("/dev/null", O_RDWR); // stdin
                    dup(fd); // stdout
                    dup(fd); // stderr

                } else {

                    // if pidfile is not an empty string (-p is used)
                    if( strncmp(pidfile,"",MAXFILEPATH) ) {
                        /* save the pid of the forked child. beware this
                           does not work with some daemons that follow up
                           with more forks. */
                        FILE *fpid = fopen(pidfile,"w");
                        if(!fpid) error("pidfile", NULL);
                        fprintf(fpid,"%u\n",pid);
                        fclose(fpid);
                    }

                    // leave us kids alone
                    _exit(0);
                }
            }
#endif

            // turn current process into the execution of command
            execv (fullcmd, &argv[optind]);
            // execv returns only on errors
            error("execv", NULL);

        }
    }

    // be polite
    fprintf(stderr,"Sorry.\n");
    exit(1);
}
