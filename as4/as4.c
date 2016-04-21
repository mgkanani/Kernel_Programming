#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
#include<linux/spinlock.h>
#include<linux/slab.h>
#include <linux/sched.h>
#include <linux/hrtimer.h>
#include <linux/cpu.h>
#include <linux/timer.h>// for timer api

#include "header.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mahendra");
MODULE_DESCRIPTION("Assignment 4:- scheduling statastics.");


unsigned int pid=0;
module_param(pid,int,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

static struct timer_list t_timer;
unsigned long s_time=2;//1 seconds

short dbg=0;
int c_id=-1;
static int is_timer_used=0;

unsigned long long rq_cpu_time=0ULL,now[2];
unsigned long ctx_cnt=0,last_nr_s[2],nr_r[2],nr_s[2],cpu_id[2];
unsigned long nr_swt[2];

//  per process stats

unsigned long long cputime[2];
unsigned long p_ctx[2];


static void display_info(unsigned long temp){
	int ret=0;
	for(ret=0;ret<2;ret++){
		//printk("cpu=%d\t now=%llu \t switches=%lu \t nr_running=%lu cputime=%llu\n",ret,now[ret],nr_s[ret]-last_nr_s[ret],nr_r[ret],cputime[ret]);
		printk("cpu=%d\t cpu_ctx_switches=%lu \t runQ_len=%lu cputime=%llu pr_ctx=%lu\n",ret,nr_s[ret]-last_nr_s[ret],nr_r[ret],cputime[ret],p_ctx[ret]);
/*		if(pid!=0){
			printk("cputime=%llu\n",cputime[ret]);
		}
*/
	}
	if(is_timer_used==1){
		ret = mod_timer( &t_timer, round_jiffies(jiffies + s_time * HZ));
		if(ret)
			printk("Error in mod_timer\n");
	}
}


static void newfunc(struct my_rq *rq, struct task_struct *tsk){
	unsigned long long delta;
	if (rq && tsk){
		c_id=rq->cpu;
		if(last_nr_s[c_id]==0){
			last_nr_s[c_id] = rq->nr_switches;
		}
		nr_r[c_id] = rq->nr_running;
		nr_s[c_id] = rq->nr_switches;
		now[c_id]  = rq->now;
		if(pid!=0 && tsk->pid==pid){
			delta = rq->now - tsk->sched_info.last_arrival;
			cputime[c_id]+=delta;
			p_ctx[c_id]++;
			printk("last_duration=%llu on cpu=%d\n",delta,c_id);
		}
/*
        	//if(tsk->pid==1368)
	        {
                	//delta = (unsigned long long)(rq->clock) - tsk->sched_info.last_arrival;
	                //rq_cpu_time += delta;
			//rq_cpu_time=tsk->sched_info.last_arrival;
			rq_cpu_time=rq->nr_switches;
			last_nr_running=rq->nr_running;
			ctx_cnt++;
			//nr_swt[rq->cpu] = rq->nr_switches;
			cpu=rq->cpu;
			if(flag==0){
				printk("cpu_id=%d\t nr_switches=%llu\n",cpu,rq_cpu_time);
				flag=1;
			}
        	}
*/
	}
}


extern void (*p_calc_cputime)(struct my_rq *, struct task_struct *);
static void (*orig_ref)(struct my_rq *rq, struct task_struct *tsk);

void init_assign(void){
	last_nr_s[0]=0;
	last_nr_s[1]=0;
	cputime[0] = 0;
	cputime[1] = 0;
	p_ctx[0]=0;
	p_ctx[1]=0;
}

int init_module(void)
{
	orig_ref    = p_calc_cputime;
	p_calc_cputime = &newfunc;

	if(is_timer_used==1){
	        init_timer(&t_timer);
        	t_timer.function=display_info;
		t_timer.data = 1;
		//t_timer.expires = jiffies + HZ;
		t_timer.expires = round_jiffies(jiffies + s_time * HZ);
		add_timer( &t_timer);
	}

	printk(KERN_INFO "Loading Dummy module\n");
    return 0;
}

void cleanup_module(void)
{
	int ret=0;
	p_calc_cputime=orig_ref;
	if(is_timer_used==1){
		ret = del_timer_sync( &t_timer );
        	if(ret)
                	printk("The timer is still in use...\n");
	}else{
		display_info(1);
	}
	printk(KERN_INFO "#### MOD-UNLOAD\n");
}

