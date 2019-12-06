#!/bin/sh
cp -rf /usr/bin/co2app /userdata/
#sleep 1
#ps -ef|grep "co2app" |grep -v grep|awk '{print $1}'|xargs kill -9
#sleep 1
chmod 777 /oem/*
chmod 777 /userdata/*
/oem/processMemScan &
function check()
{
restart_time=0
while true
do
	count=`ps -ef |grep $1 |grep -v "grep" |wc -l`
	echo $count
	if [ 0 == $count ];then
 		restart_time=$(($restart_time+1))
		echo "restart_time="$restart_time > /data/restart_time
		$1 &
	fi
	sleep 1
done
}
/usr/bin/HiSound &
check /usr/bin/HiSound


