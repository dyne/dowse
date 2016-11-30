#!/usr/bin/env zsh
source dowse
echo "sql=$sql"
while true ; do
   ./src/dowse-cmd-fifo| awk -F, '{ print "UPDATE found SET state=\47" $3 "\47 WHERE (( macaddr=\47" $5 "\47 and \47" $5 "\47 <> \47\47) or \
 ( ip4=\47" $6 "\47 and \47" $6 "\47 <> \47\47) or\
 ( ip6=\47" $7 "\47 and \47" $7 "\47 <> \47\47)) ;"}'    |  $sql things
done
