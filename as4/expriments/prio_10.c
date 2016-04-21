#include<sys/resource.h>
#include<stdio.h>

int main(){
	int which,who,prio,ret,t_pid;
        unsigned int ui=0,max=1000000009,iter=0,run=0;
	id_t my_pid; 
	my_pid = getpid();
	printf("parent=%d\n",my_pid);
	sleep(5);
	which = PRIO_PROCESS;
	prio=10;
	int pid=fork();
	if(pid==0){
	//child
		//my_pid = getpid();
		//printf("child=%d\n",my_pid);
		//prio=-20;
		//ret=setpriority(which,my_pid,prio);
		t_pid=fork();
		while(1);
	}else{
	//parent
	}
//        while(run<5){
		setpriority(PRIO_PROCESS,0,prio);
	        for(iter=0;iter<=max;iter++){
        	        ui+=iter;
                	ui%=max;
	        }
        	run++;
		//prio-=10;
		sleep(4);
//	}
        return 0;
}
