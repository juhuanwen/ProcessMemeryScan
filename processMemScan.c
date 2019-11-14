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

#define DEBUG_TEST
#ifdef DEBUG_TEST
    #define LOG printf("%s|%04d|", __FILE__, __LINE__); printf
#else
    #define LOG printf
#endif
#define ProcessName "test" //进程在系统中的名称
#define ProcessMaxMem 10240//最大值，超过这个值，重新启动源程序 512000--500M
#define BinRootCross "/home/samba/NewDisk1/procesMemScan/test"//进程的路径，便于再次重启调用
#define Cycle 3 //判断周期
void GetProcessIdByName(char * Name,char * ID);
int GetMemByID(char * ID);	
char *Mytrum(char *ptr);

int main()
{	int i=0;
	char commandBuf[200]={0};
	int memSize=0;//进程占用的内存大小
	static char id[20]={0};//存储要检测的进程ID
	while(1){
		GetProcessIdByName(ProcessName,id);//获取达到进程ProcessName的ID
		if(id!=0){
			memSize=GetMemByID(id);//通过ID获取到当前使用的内存
			if(memSize>ProcessMaxMem){
				LOG("memSize is too big ,kill it!\n");
				Mytrum(id);//删除字符串中的所有空格
				for(i=0;i<strlen(id);i++){//取消掉回车符
					if(*(id+i)==0x0a){
						*(id+i)=0;
						break;
					}
				}
				sprintf(commandBuf,"kill %s",id);
				system(commandBuf);//关闭进程
				sleep(2);//关闭进程后等待2秒
				sprintf(commandBuf,"%s&",BinRootCross);
				system(commandBuf);//重启进程
				
			}else{
				sleep(Cycle);
			}
		}else{
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
    fgets(config_buf,sizeof(item[0]),Pro_fp); //读取一行
    pclose(Pro_fp);
	for(i=0;i<strlen(config_buf);i++){//取消掉回车符
		if(*(config_buf+i)==0x0a){
			*(config_buf+i)=0;
			break;
		}
	}
    LOG("the ProcessID=%s\n",config_buf);
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
	sprintf(item[1],"cat /proc/%s/status | grep VmRSS | grep -v 'grep' | awk '{print $2;}'",ID);
	Pro_fp=popen(item[1],"r");
	fgets(config_buf,sizeof(item[0]),Pro_fp);
	pclose(Pro_fp);
	memSize=atoi(config_buf);
	LOG("the memSize=%d\n",memSize);
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
