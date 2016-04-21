if [ $# -ne 2 ]
then
	echo "usage:- sh script.sh major_no minor_no";
	exit 0;
fi
dev=`cat /proc/devices |grep mydev`

if [ ! -z "$dev" ]
then
	echo $dev;
	sudo mknod /dev/mydev c $1 $2
	sudo chmod a+r+w /dev/mydev
else
	echo "insert module then run this script"
fi
