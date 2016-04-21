# clears the page cache
#dd if=/dev/zero of=file bs=4k count=`cat /proc/meminfo |grep "^Cached"|cut -d":" -f2 |tr -s [:blank:]|cut -d" " -f2`

#This will take around 60sec.
#creates ~80MB file
#dd if=/dev/urandom of=file_1 bs=80M count=1
dd if=/dev/urandom of=rfile_1 bs=20M count=4
#dd if=/dev/zero of=file bs=80M count=1


for i in {1..20}; 
do
#reading the file
#block size is 16bytes so the hit count will be too high.
dd if=rfile_1 bs=16 count=5000000 of=wfile_1
done
