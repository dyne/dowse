#!/usr/bin/env zsh

# This are the schemes of dowse databases and lists in redis.

# This information is used to generate code that is then included at
# compile time, so any change of it requires a recompilation.


R=`pwd`/..
[[ -r $R/src ]] || {
    print "error: config.sh must be run from the src"
    return 1
}

mkdir -p $R/run

zkv=1
source $R/zlibs/zuper
vars=(tmp)
maps=(db mod execmap execrules)
source $R/zlibs/zuper.init

source paths.sh

# for the db keys namespace see doc/HACKING.md

dbindex='
0 dynamic
1 runtime
2 storage
'


### Database index

# save databases for the shell scripts
for i in ${(f)dbindex}; do
    # this is reverse order: names are the indexes
    db+=( ${i[(w)2]} ${i[(w)1]} )
done
zkv.save db $R/src/database.zkv

# save databases for the C code
rm -rf $R/src/database.h
touch  $R/src/database.h
for i in ${(k)db}; do
    print "#define db_$i ${db[$i]}" >> $R/src/database.h
done






### Modules index

# # save modules for the shell scripts
# mod=()
# for i in ${(f)modcodes}; do
#     mod+=( ${i[(w)2]} ${i[(w)1]} )
# done

# # save modules for the C code
# rm -rf $R/src/module.h
# touch  $R/src/module.h
# for i in ${(k)mod}; do
#     print "#define mod_$i ${mod[$i]}" >> $R/src/module.h
# done

# mod=()
# for i in {1..${(f)#modfields}}; do
#     mod+=( $i ${modfields[$i]} )
# done
# zkv.save mod $R/src/module.zkv


print "# Compile-time configuration generated"
