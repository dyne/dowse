mkdir record

while true ; do 
TMP=`date +%Y%m%d_%H%M%S`

lsof -iTCP -n > record/$TMP
sleep 10
done
