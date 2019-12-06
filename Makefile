#KERNEL_DIR=
all:
	/home/vmuser/SME/buildroot/output/rockchip_rk3308_release/host/bin/aarch64-rockchip-linux-gnu-gcc processMemScan.c -o processMemScan -lpthread
	/home/vmuser/SME/buildroot/output/rockchip_rk3308_release/host/bin/aarch64-rockchip-linux-gnu-gcc co2app.c -o co2app -lpthread
	/home/vmuser/SME/buildroot/output/rockchip_rk3308_release/host/bin/aarch64-rockchip-linux-gnu-gcc test.c -o test -lpthread
clean:
	rm -rf *.o processMemScan
cp:
	cp processMemScan test /media/sf_linux_share/
