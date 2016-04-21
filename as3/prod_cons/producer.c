#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
int main() {
	int fd=0,ret=0,i=0,val=0;
//	char buf[1024]="";
	fd=open("/dev/mydev",O_WRONLY);
	printf("fd:%d\n",fd);
	for(i=0;i<2570000;i++){
		ret=write(fd,(const void *)&i,sizeof(int));
		if(ret==0)
			sleep(1);
		//sleep(1);
	//	while(!ret)
	//		ret=write(fd,(const void *)&i,sizeof(int));
	}
	return 0;
}
