/*************************************************************************
	> File Name: main.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2018年11月21日 星期三 16时14分28秒
 ************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
void delay(int i)
{
	unsigned long x=0,y=0;
	for(x=5000;x>0;x--)
		for(y=i;y>0;y--);
}
int main(int argc,char **argv)
{
	int fd=-1;
	int led_no=0;
	/*
 	if(argc != 3)
 	{
  		printf("Usage:%s \n",argv[0]);
  		exit(0);
 	}
	*/
	fd=open("/dev/led",0);				//打开驱动文件
	int fd1=0;
	fd1=open("/dev/led2",0);
	if(fd<0)
	{
		printf("open error\n");
		exit(0);
	}
	/*
	led_no=strtoul(argv[1],0,0)-1;
	if(!strcmp(argv[2],"on"))
		ioctl(fd,0,led_no);
	else if(!strcmp(argv[2],"off"))
		ioctl(fd,1,led_no);
	else
		exit(0);
	*/
	int i=0,t=1000,t2=0;
	printf("请输入led1和led2的时间间隔(ms):\n");
	scanf("%d",&t);
	printf("请输入led13和led4的时间间隔(ms):\n");
	scanf("%d",&t2);
	while(1)
	{
		for(i=0;i<2;i++)
		{
			ioctl(fd,0,i);
			delay(t);

			ioctl(fd,1,i);
			delay(t);
		}
		for(i=2;i<4;i++)
		{
			ioctl(fd1,0,i);
			delay(t2);

			ioctl(fd1,1,i);
			delay(t2);
		}
	}
	return 0;
}
