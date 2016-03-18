# sup
## a "small is beautiful" tool for UNIX privilege escalation

sup is a very small and secure C application. it is designed to run as root (with suid bit on) to facilitate the privilege escalation of users to execute certain programs as superuser.

all settings in sup are hard-coded at compile time. sup is very portable and self-contained, designed for production use as a static binary. sup is a sort of hard-coded sudo: it is an ideal companion for artisans building small containers and embedded systems.

the latest version of sup can be found on https://git.devuan.org/jaromil/sup and on its mirrors at dyne.org and github.com.

## why are you whispering?

as you may have realised already, sup is so minimal that all its documentation is written lowercase. it was originally written in 2009 by pancake of nopcode.org and maintained until 2011 as part of the suckless tools. in 2016 sup is being adopted by jaromil of dyne.org, extending its features to support static build with [http://www.musl-libc.org/](musl-libc) and hardcoded sha256 hashing of binary files.

## configure

sup's configuration resides in config.h and should be set before
building. here below an intuitive example

```c
#define USER 1000
#define GROUP -1

#define SETUID 0
#define SETGID 0

#define CHROOT ""
#define CHRDIR ""

#define HASH 1

#define ENFORCE 1

static struct rule_t rules[] = {
    // allow user to run these programs when found in path location
    { USER, GROUP, "whoami",   "/usr/bin/whoami", "" },
    { USER, GROUP, "ifconfig", "/sbin/ifconfig",  "" },
    { USER, GROUP, "ls",       "/bin/ls",         "" },
    { USER, GROUP, "wifi",     "/root/wifi.sh",   "" },
    // allow to run id when found in PATH with matching hash
    { USER, GROUP, "id",       "*", "db533b77fc9e262209a46e0f.." },
     // allow to run any program found in PATH
    { USER, GROUP, "*",        "*"},
    { 0 }, // end of configuration
};
```
Fields are organized as following:

| USER | GGROUP | binary name | binary path | hash (optional) |

- `USER`  is the numeric id of the user authorized to execute the binary as superuser
- `GROUP` is the numeric group  of the user authorized to execute the binary as superuser (-1 for none)
- `hash` can be computed before build using sha256sum (GNU coreutils)

using `sup -l` the configuration can be displayed at runtime.

## build

sup requires a C compiler and the GNU make tool to be built.

a simple `make` command will build a sup binary good enough for
evaluation purposes, with dynamic links to the libm and libc libraries
installed system-wide.

for production use sup should be built as a static binary: this is
easily done by first installing musl-libc in its default location and
then using the `make musl` command.

## technical details

sup consists of 3 files: sup.c is the main source and config.h is the
configuration, hardcoded at compile time. sha256.c is optional and
provides the hashing functionality if the define HASH is set.

sup is written in ANSI C with POSIX1.b compliancy for GNU/Linux and
BSD systems. it uses `setuid/gid` for privilege escalation and
`execv()` to launch processes as superuser.

## licensing

sup is copyleft software licensed as GNU Lesser Public License
(LGPLv3). when compiled with hashing capability, its license turns
into GNU GPLv2 because of the sha256 component.

```
sup is copyleft (c) 2009-2011 by pancake of nopcode.org
                (c) 2016      by jaromil of dyne.org

the FIPS-180-2 sha-256 implementation optionally included in sup is
copyleft (c) 2001-2003 by Christophe Devine
```
