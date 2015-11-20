#!/usr/bin/env zsh

# ZSh script to quickly import all the database into Redis
# All domain entries are created as keys, their value is the file containing them

R=`pwd`

[[ -r $R/scripts ]] || {
    print "Error: run this script from the base directory, i.e:"
    print "./scripts/import-in-redis.sh"
    return 1 }

print "Importing $R/data into local Redis"

en=0
ln=0
value=""
for list in "${(f)$(find $R/data -type f)}"; do
    
    for entry in "${(f)$(cat $list| grep -v -e '^#' -e '^$')}"; do

        [[ "${(w)entry[1]}" = " " ]] && continue

        value=`basename $list`
        print "SET ${entry:l} ${value:l}" | redis-cli 1> /dev/null
        [[ $? = 0 ]] || {
            print "Error: redis-cli returns $?"
            return 1 }
        en=$(( $en + 1 ))

    done

    ln=$(( $ln + 1 ))
done

print "Done! processed $en entries in $ln lists."
return 0
