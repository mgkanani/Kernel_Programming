#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
int buf=0;
int main() {
	int fd=0,ret=0,i=0,val=0;
//	char buf[1024]="";
	fd=open("/dev/mydev",O_RDONLY);
	printf("fd:%d\n",fd);
	for(i=0;i<2570000;i++){
		ret=read(fd,(void *)&val,sizeof(int));
		if(ret)
			printf("ret=%d\tval=%d\n",ret,val);
                else
                        sleep(1);
                //sleep(1);
	}
//	buf[ret]='\0';
//	printf("buff:%s , len:%d bytes\n",buf,ret);
	return 0;
}
