#!/usr/bin/env zsh

# This are the schemes of dowse databases and lists in redis.

# This information is used to generate code that is then included at
# compile time, so any change of it requires a recompilation.


### db keys namespace
#
# static : conf_ modules_ daemons_
#
# dynamic: obj_ dns_
# |___dns: dns_query_channel dns_query_fifo
# |___obj: obj_add_channel obj_add_fifo
# |        obj_rem_channel obj_rem_fifo
# X

dbindex='
1 runtime
2 dynamic
3 storage
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


print "Database indexes generated"
