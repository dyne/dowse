#!/usr/bin/env zsh

# This are the schemes of dowse databases and lists in redis.

# This information is used to generate code that is then included at
# compile time, so any change of it requires a recompilation.


dbcodes='
0 conf
1 leases
2 modules
3 daemons
4 domains
5 dnsqueries
'

modcodes='
0 name
1 desc
2 tags
3 ports
4 daemons
5 authors
6 version
'

R=`pwd`
[[ -r $R/src ]] || {
    print "error: database.sh must be run from the source base dir"
    return 1
}

zkv=1
source $R/zlibs/zuper
maps=(db mod)
source $R/zlibs/zuper.init

for i in ${(f)dbcodes}; do
    db+=( ${i[(w)2]} ${i[(w)1]} )
done

# save it for the shell scripts
zkv.save db $R/src/database.zkv

for i in ${(f)modcodes}; do
    mod+=( ${i[(w)2]} ${i[(w)1]} )
done

# save it for the shell scripts
zkv.save mod $R/src/module.zkv


# save it for the C code
rm -rf $R/src/database.h
touch  $R/src/database.h
for i in ${(k)db}; do
    print "#define db_$i ${db[$i]}" >> $R/src/database.h
done

# save it for the C code
rm -rf $R/src/module.h
touch  $R/src/module.h
for i in ${(k)mod}; do
    print "#define mod_$i ${mod[$i]}" >> $R/src/module.h
done

print "Database indexes generated"
