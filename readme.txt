判断某个进程是否超过约定的内存占用，如果超过就关闭重启该进程
test.c为内存申请程序，不断申请
processMemScan.c为监控程序
添加了qDebug.txt文件的增长检测，当15s以内没有任何增长则关闭co2app重启,在/home/vmuser/SME/device/rokdev/rk3308/oem目录下修改了初始化脚本init.Server.sh