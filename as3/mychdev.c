#include<linux/proc_fs.h>//for procfs 
#include<linux/seq_file.h>//for procfs 
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/fs.h> //for register_chrdev
#include<asm/uaccess.h> // only for copy_from_user

#include <linux/kfifo.h>
#include<linux/spinlock.h>

#define MODULE_NAME "ASG3"
#define Q_SIZE 1048576 //2^20 it must be in power of 2.

int dbg = 0;//make it 1 for enabling debug mode.

static int major=0;
//static char msg[100]={0};
static unsigned int r_cnt=0;
static unsigned int w_cnt=0;

spinlock_t my_lock;


static int dev_open(struct inode *, struct file *);
static ssize_t dev_read(struct file *filp, char __user *buf , size_t len, loff_t *offset);
static ssize_t dev_write(struct file *filp, const char __user *buf , size_t len, loff_t *offset);
static int dev_close(struct inode *, struct file *);

static struct file_operations fops={
	.open=dev_open,
	.read=dev_read,
	.write=dev_write,
	.release=dev_close,
};

static int my_proc_open(struct inode *inode, struct file *filep);
static const struct file_operations proc_fops={
	.owner = THIS_MODULE,
	.open = my_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};
static struct proc_dir_entry *proc_file_entry;

static struct kfifo QUE;

int create_Q(void) {
        int ret;
        ret = kfifo_alloc(&QUE,Q_SIZE,GFP_KERNEL);
        if(!ret)
                printk("QUEUE created of size %u\n",kfifo_size(&QUE));
	else
                printk("Err in creating QUEUE\n");
        return ret;
}


int create_procfs(void);
void remove_procfs(void);

int init_module(void) {
	int ret=0;
	ret=create_Q();
	if(ret)
		return ret;
	major = register_chrdev(0,"mydev",&fops);
	printk("LOADING KERNEL MODULE\n");
	if(major<0) {
		printk("Registering character device failed,major=%d\n",major);
		return major;
	}
	printk("Registering Character device succeed, major=%d\n",major);
	ret=create_procfs();
	if(ret<0){
		unregister_chrdev(major,"mydev");
		return ret;
	}
	printk("Procfs initialization succeed\n");
	return 0;
}

void cleanup_module(void) {
	kfifo_free(&QUE);//unallocate queue.
	remove_procfs();
	unregister_chrdev(major,"mydev");
	printk("Total read %u \t Total write %u\n",r_cnt,w_cnt);
	printk("UNLOADING KERNEL MODULE\n");
}


static int dev_open(struct inode *inode, struct file *filep) {
	if(dbg)
		printk("mydev Device opened\n");
	return 0;
}

static ssize_t dev_read(struct file *filp, char __user *buf , size_t len, loff_t *offset) {
	//int ret=0,copied=0;
	int ret=0,tmp=0;
	if (kfifo_is_empty(&QUE)) {
		if(dbg)
			printk("QUEUE empty\n");
		return 0;
	}
	//ret = kfifo_to_user(&QUE, buf, len, &copied);
	ret=kfifo_out_spinlocked(&QUE,(void *)buf,sizeof(int),&my_lock);
	//ret=kfifo_out(&QUE,(void *)buf,sizeof(char));
	if(dbg){
		tmp=(int)*buf;
		printk("%i\n",tmp);
	}
	if(ret>0)
		r_cnt++;
	return ret;
}

static ssize_t dev_write(struct file *filp, const char __user *buf , size_t len, loff_t *offset) {
	int ret=0,i_size=sizeof(int);
	char msg[4];
	if(kfifo_is_full(&QUE)){
		if(dbg)
			printk("QUEUE full\n");
		return 0;
	}
	if(dbg){
		ret=(int)*buf;
		printk("%i\n",ret);
	}
	if(len<i_size){
		ret=0;
		while(ret<len){
			msg[ret]=buf[ret];
			ret++;
		}
		while(ret<i_size){
			msg[ret]='\0';
			ret++;
		}
		ret=kfifo_in_spinlocked(&QUE,(const void *)&msg,sizeof(int),&my_lock);
	}
	else{
		ret=kfifo_in_spinlocked(&QUE,(const void *)buf,sizeof(int),&my_lock);
	}
	//ret=kfifo_in(&QUE,(const void *)buf,sizeof(char));
	if(ret)
		w_cnt++;
	return ret;
}


static int dev_close(struct inode *inode, struct file *filep) {
	if(dbg)
		printk("mydev Device closed\n");
	return 0;
}

static int read_proc(struct seq_file *f, void *v) {
	char *tr="true\0";
	char *fl="false\0";
	char *full=NULL;
	char *empty=NULL;
	unsigned int size=(Q_SIZE/sizeof(int));
	full=kfifo_is_full(&QUE)?tr:fl;
	empty=kfifo_is_empty(&QUE)?tr:fl;
	seq_printf(f,"total read=%u \t\t total writes=%u\nis_empty=%s \t\t is_full=%s\nQUEUE_Size(B)=%u \t\t Filled_Data_size(B)=%u\nTotal_blocks=%u \t filled_blocks=%u \t\t avail_blocks=%u\nhead_location=%u \t\t tail_location=%u\n",r_cnt,w_cnt,empty,full, (Q_SIZE), (kfifo_len(&QUE)), (Q_SIZE/sizeof(int)), (kfifo_len(&QUE)/sizeof(int)),((Q_SIZE/sizeof(int)) - (kfifo_len(&QUE)/sizeof(int))), ((QUE.kfifo.in/sizeof(int))%size),((QUE.kfifo.out/sizeof(int))%size));
	return 0;
}

static int my_proc_open(struct inode *inode, struct file *filep) {
	return single_open(filep, read_proc, NULL);
}


int create_procfs(void) {
	proc_file_entry = proc_create("myprocfs", 0, NULL, &proc_fops);
	if(proc_file_entry==NULL)
		return -1;
	return 0;
}

void remove_procfs(void) {
	//remove_proc_entry("myprocfs",proc_file_entry);
	remove_proc_entry("myprocfs",NULL);
}

/*
References:-
http://stackoverflow.com/questions/25746461/error-implicit-declaration-of-function-create-proc-read-entry-werror-implic --deprecated function
https://lkml.org/lkml/2013/4/11/215

http://stackoverflow.com/questions/8516021/proc-create-example-for-kernel-module

*/
