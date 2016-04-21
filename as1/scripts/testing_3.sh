# clears the page cache
#dd if=/dev/zero of=file bs=4k count=`cat /proc/meminfo |grep "^Cached"|cut -d":" -f2 |tr -s [:blank:]|cut -d" " -f2`

#This will take around 60sec.
#creates ~80MB file
#dd if=/dev/urandom of=file bs=20M count=1
dd if=/dev/urandom of=rfile_3 bs=5M count=4
#dd if=/dev/zero of=file bs=80M count=1


for i in {1..20}; 
do
#reading the file
#block size is 4bytes so the hit count will be too high.
dd if=rfile_3 bs=4 count=5000000 of=wfile_3
done
