#!/usr/bin/env zsh

# This are the schemes of dowse databases and lists in redis.

# This information is used to generate code that is then included at
# compile time, so any change of it requires a recompilation.

S=${$(pwd)%/*}
[[ -r $S/src ]] || {
    print "error: config.sh must be run from the src"
    return 1
}


zkv=1
source $S/zuper/zuper
vars=(dbindex thingindex)
maps=(db)
source $S/zuper/zuper.init

fn config $*

PREFIX="$1"
req=(PREFIX)
ckreq || return 1


#########
dbindex='
0 dynamic
1 runtime
2 storage
'
# for the db keys namespace see doc/HACKING.md
#########

mkdir -p $S/build/db

# index of all database fields for a thing in the index
# fields to the right of semicolon are for parsing the nmap xml output
# TODO: perhaps no need to write this as a file, just print and pipe

# id     SQL                       NMAP
thingindex='
macaddr  ; /nmaprun/host[\$i]/address[@addrtype=\"mac\"]/@addr
ip4      ; /nmaprun/host[\$i]/address[@addrtype=\"ipv4\"]/@addr
hostname ; /nmaprun/host[\$i]/hostnames/hostname[0]/@name
os       ; /nmaprun/host[\$i]/os/osmatch[1]/@name
vendor   ; /nmaprun/host[\$i]/address[@addrtype=\"mac\"]/@vendor
'
print - "$thingindex" > $S/build/db/thing.idx

### Database index

# save databases for the shell scripts
for i in ${(f)dbindex}; do
    # this is reverse order: names are the indexes
    db+=( ${i[(w)2]} ${i[(w)1]} )
done
zkv.save db $S/build/db/database.zkv

# save databases for the C code
rm -rf $S/src/database.h
touch  $S/src/database.h
for i in ${(k)db}; do
    print "#define db_$i ${db[$i]}" >> $S/src/database.h
done

# db=()
# for i in ${(f)thingindex}; do
#     db+=( ${i[(w)1]} ${i[(w)1]} )
# done

rm -rf $S/src/thingsdb.h
touch $S/src/thingsdb.h
c=1
#print "#define THINGS_DB \"$db[things]\"" > $S/src/thingsdb.h

# for i in "${(f)thingindex}"; do
#     [[ "$i" = "" ]] && continue
#     print "#define ${i[(w)1]} $c" >> $S/src/thingsdb.h
#     c=$(( $c + 1 ))
# done

print "#define all_things_fields $(( $c - 1 ))" >> $S/src/thingsdb.h

# print "static char *get[] = {" >> $S/src/thingsdb.h
# for i in "${(f)thingindex}"; do
#     [[ "$i" = "" ]] && continue
#     print "\"${i[(w)1]}\"," >> $S/src/thingsdb.h
# done
# print " NULL };" >> $S/src/thingsdb.h

notice "Database indexes generated"

#####
_id=`id -un`
_gid=`id -gn`
cat <<EOF > privileges
dowse_uid=$_id
dowse_gid=$_gid
EOF
notice "Configured privileges for caller: $_id $_gid"
######
