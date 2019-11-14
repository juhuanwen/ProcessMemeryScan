#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void Sleep(int ms)
{
	struct timeval delay;
	delay.tv_sec = 0;
	delay.tv_usec = ms * 1000; // 20 ms
	select(0, NULL, NULL, NULL, &delay);
}

int main()
{
	unsigned char * p;
	int i=0;
	while(1){//不断申请内存
		Sleep(1);
		p=malloc(1024*1024*sizeof(char));
		memset(p,0xff,1024*sizeof(char));
	}
}