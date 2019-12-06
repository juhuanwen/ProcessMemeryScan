/*
VmSize(KB) ：虚拟内存大小。整个进程使用虚拟内存大小，是VmLib, VmExe, VmData, 和 VmStk的总和。
VmRSS(KB)：虚拟内存驻留集合大小。这是驻留在物理内存的一部分。它没有交换到硬盘。它包括代码，数据和栈。
VmData(KB)： 程序数据段的大小（所占虚拟内存的大小），堆使用的虚拟内存。
VmStk(KB)： 任务在用户态的栈的大小，栈使用的虚拟内存

VmExe(KB)： 程序所拥有的可执行虚拟内存的大小，代码段，不包括任务使用的库

VmLib(KB) ：被映像到任务的虚拟内存空间的库的大小  143168 455440
*/

/*
通过检测某个进程(ProcessName)的内存占用情况决定其是否超过内存范围(ProcessMaxMem(KB)),如果超过则关闭进程重新开启BinRootCross(进程路径)
需要修改的值如下：
ProcessName    进程在系统中的名称
ProcessMaxMem  最大值，单位为KB，超过这个值，重新启动源程序 512000--500M
BinRootCross    进程的路径，便于再次重启调用
Cycle			判断周期，默认3秒
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define DEBUG_TEST
#ifdef DEBUG_TEST
    #define LOG printf("%s|%04d|", __FILE__, __LINE__); printf
#else
    #define LOG printf
#endif
#define ProcessName "co2app" //进程在系统中的名称
#define ProcessMaxMem 460800//最大值，超过这个值，重新启动源程序 512000--500M 460800--450M  430080--420M 450560-440M
//#define BinRootCross "/oem/co2app -platform linuxfb >> ScanLog &"//进程的路径，便于再次重启调用
#define Cycle 3 //判断周期
void GetProcessIdByName(char * Name,char * ID);
int GetMemByID(char * ID);	
char *Mytrum(char *ptr);
long long GetFileLines(char * Name);

int main()
{	int i=0,j=0;
	char commandBuf[200]={0};
	int memSize=0;//进程占用的内存大小
    char id[20]={0};//存储要检测的进程ID
	unsigned char isExit=0;
	long long fileLines=0;
	long long fileLines1=1;
	system("date");

	while(1){
		for(i=0;i<20;i++){
			id[i]=0;
		}
		isExit=0;
		GetProcessIdByName(ProcessName,id);//获取达到进程ProcessName的ID
		for(i=0;i<20;i++){
			if(id[i]!=0){
				isExit=1;
				break;
			}
		}
		
		if(isExit==1){
			
			memSize=GetMemByID(id);//通过ID获取到当前使用的内存
			Mytrum(id);//删除字符串中的所有空格
				for(i=0;i<strlen(id);i++){//取消掉回车符
					if(*(id+i)==0x0a){
						*(id+i)=0;
						break;
					}
				}
			if(memSize>ProcessMaxMem){
				
				LOG("memSize is too big ,kill it!\n");
				LOG("memSize is %d,ProcessID=%s ,kill it!\n",memSize,id);
				sprintf(commandBuf,"kill -s 9 %s",id);
				system(commandBuf);//关闭进程
				sleep(1);//关闭进程后等待1秒
				// sprintf(commandBuf,"%s",BinRootCross);
				// system(commandBuf);//重启进程
				system("chmod 777 /oem/*");
				system("chmod 777 /userdata/*");
				system("/userdata/co2app -platform linuxfb &");
				system("date");
			}else{
				sleep(Cycle);
				j++;
				if(j==5){
					fileLines=GetFileLines("/userdata/qDebug.txt");
					//printf("\r\nfileLines=%lld\r\n",fileLines);
					if(fileLines1!=fileLines){
						fileLines1=fileLines;
					}else{//超过15秒文件qDebug.txt行数没有变化,代表co2app死了
						//printf("\r\nqDebug.txt is not change for 15 senconds\r\n");
						sprintf(commandBuf,"kill -s 9 %s",id);
						system(commandBuf);//关闭进程
						sleep(1);//关闭进程后等待1秒
						// sprintf(commandBuf,"%s",BinRootCross);
						// system(commandBuf);//重启进程
						system("chmod 777 /oem/*");
						system("chmod 777 /userdata/*");
						system("/userdata/co2app -platform linuxfb &");
						system("date");
					}
					//system("date");
					//LOG("memSize is %d,ProcessID=%s \n",memSize,id);
					j=0;
				}
				
				
			}
		}else{
			//LOG("No this Process:%s exsist",ProcessName);
			system("date");
			system("/userdata/co2app -platform linuxfb &");
			sleep(Cycle);
		}
	}
}
/*
通过程序的名称来获得程序的ID号码
para0:名称，字符串
para1:用用于接收ID的字符串指针，10个字符就够
*/
void GetProcessIdByName(char * Name,char * ID)
{
	static char  item[2][200] ={0};
	char *config_buf = item[0];
	int i=0;
	FILE *Pro_fp;
	sprintf(item[1],"ps | grep %s | grep -v 'grep' | awk '{print $1;}'",Name);
	Pro_fp=popen(item[1],"r");
    if(fgets(config_buf,sizeof(item[0]),Pro_fp)==NULL){ //读取一行
		pclose(Pro_fp);
		//LOG("the process is not exit\r\n");
		return;
	}
    pclose(Pro_fp);
	for(i=0;i<strlen(config_buf);i++){//取消掉回车符
		if(*(config_buf+i)==0x0a){
			*(config_buf+i)=0;
			break;
		}
	}
    //LOG("the ProcessID=%s\n",config_buf);
	strcpy(ID,config_buf);
}
/*
通过进程号获取当前进程所占用内存大小
para0:进程号，字符串格式
返回当前进程所占用内存KB
*/
int GetMemByID(char * ID)
{
	int memSize=0,i=0;
	static char item[2][200]={0};
	char *config_buf = item[0];
	FILE *Pro_fp;
	Mytrum(ID);//删除字符串中的所有空格
	for(i=0;i<strlen(ID);i++){//取消掉回车符
		if(*(ID+i)==0x0a){
			*(ID+i)=0;
			break;
		}
	}
	sprintf(item[1],"cat /proc/%s/status | grep VmSize | grep -v 'grep' | awk '{print $2;}'",ID);
	Pro_fp=popen(item[1],"r");
	fgets(config_buf,sizeof(item[0]),Pro_fp);
	pclose(Pro_fp);
	memSize=atoi(config_buf);
	//LOG("the memSize=%d\n",memSize);
	return memSize;
}
/*
去掉字符串中的空格
*/
char *Mytrum(char *ptr)
{
    char *p,*q;
    for(p=ptr;*p!='\0';)
    {
        q=p;
        if(*q==' ')
        {
            while(*q!='\0')
            {
                *q=*(q+1);
                q++;
            }
        }
        else  
            p++;
    }
    return ptr;
}

long long mux10(int i)
{
	int j=0;
	long long res=1;
	for(j=0;j<i-1;j++)
	{
		res*=10;
	}
	return res;
}

long long String2Int(char * buf)
{
	int len=strlen(buf);
	int i=0;
	unsigned char ss=0;
	long long length=0;//返回多少行
//	printf("the length is %d\r\n",len);
	
	for(i=0;i<len;i++)
	{
		ss=*(buf+i);
		//printf("the first is %x\r\n",ss);
		if((ss<='9')&&(ss>='0')){//每一个ASCII位于0-9间才有效
			length+=(ss&0x0f)*mux10(len-i);
		}
	}
	//printf("length=%lld\n",length);
	return length;
}
/*
获取某个文件的总行数
para0:文件名
res:返回当前文件有多少行数据
*/
long long GetFileLines(char * Name)
{
	static char  item[2][200] ={0};
	char *config_buf = item[0];
	int i=0;
	FILE *Pro_fp;
	sprintf(item[1],"wc -l %s | awk '{print $1;}'",Name);
	Pro_fp=popen(item[1],"r");
    if(fgets(config_buf,sizeof(item[0]),Pro_fp)==NULL){ //读取一行
		pclose(Pro_fp);
		//LOG("the process is not exit\r\n");
		return 0;
	}
    pclose(Pro_fp);
	for(i=0;i<strlen(config_buf);i++){//取消掉回车符
		if(*(config_buf+i)==0x0a){
			*(config_buf+i)=0;
			break;
		}
	}
    //LOG("the file line is =%s\n",config_buf);
	return  String2Int(config_buf);
	
}

// void main()
// {
	// long long FileLength=0;
	// FileLength=GetFileLines("qDebug.txt");
	// LOG("the file length is %lld\r\n", FileLength);
// }

