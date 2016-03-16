#!/usr/bin/env zsh

R=${$(pwd)%/*}
[[ -r $R/src ]] || {
    print "error: config.sh must be run from the src"
    return 1
}

zkv=1
source $R/zlibs/zuper
vars=(tmp)
maps=(execmap execsums execrules)
source $R/zlibs/zuper.init

source paths.sh
zkv.save execmap $R/src/execmap.zkv

act "generating execution rules"

builduid=`id -u`
buildgid=`id -g`
# generate configuration for sup
cat <<EOF > $R/src/execrules.h
#define USER $builduid
#define GROUP $buildgid

#define SETUID 0
#define SETGID 0

#define CHROOT ""
#define CHRDIR ""

#define ENFORCE 1

static struct rule_t rules[] = {
EOF

for x in ${(k)execmap}; do
    if [[ "$execrules[$x]" = "user" ]]; then
        print - "{ USER, GROUP, \"$x\", \"$execmap[$x]\" }," >> $R/src/execrules.h
    elif [[ "$execrules[$x]" = "root" ]]; then
        print - "{ SETUID, SETGID, \"$x\", \"$execmap[$x]\" }," >> $R/src/execrules.h
    fi
done

cat <<EOF >> $R/src/execrules.h
{ 0 },
};
EOF


notice "Computing checksums..."

execsums=()
for x in ${(v)execmap}; do
    sum=`sha256sum $x`
    sum=${sum[(w)1]}
    [[ "$sum" = "" ]] && {
        warning "missing checksum for: $x"
        continue }
    execsums+=($x $sum)
    act "$sum $x"
done
zkv.save execsums $R/src/execsums.zkv

notice "Dowse build complete on `hostname` (`date`)"
