// sup's configuration file
// need sup to be re-compiled for any change to be effective

/// un/comment flags below to remove functionalities
#define HASH 1
#define DAEMON 1
// #define DEBUG 1

#ifndef FLAGSONLY

#define USER 1000
#define GROUP -1

#define SETUID 0
#define SETGID 0

#define CHROOT ""
#define CHRDIR ""

static struct rule_t rules[] = {
    // allow user to run these programs when found in path location
    { USER, GROUP, "whoami",   "/usr/bin/whoami", "" },
    { USER, GROUP, "ifconfig", "/sbin/ifconfig",  "" },
    { USER, GROUP, "ls",       "/bin/ls",         "" },
    { USER, GROUP, "wifi",     "/root/wifi.sh",   "" },

    // allow to run this program when found in PATH */
    { USER, GROUP, "id", "*", "db533b77fc9e262209a46e0fe5bec646c1d2ed4e33285dc61da09dbc4caf6fa6" },
    /* { USER, GROUP, "*", "*"}, // allow to run any program found in PATH */
    { 0 },
};

#endif
