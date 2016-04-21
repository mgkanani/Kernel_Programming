#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
#include <linux/fs.h>	     // for inode data structure.
#include<linux/spinlock.h>
#include<linux/slab.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mahendra");
MODULE_DESCRIPTION("Page Cache Hit counter.");

struct cache_hits{
	unsigned long inode;
	unsigned long cnt;
	struct cache_hits *next;
};
short dbg=0;
static struct cache_hits *head=NULL;
//static int total=0;
static int f_cnt=0;
static rwlock_t  i_lock;

struct cache_hits* insert(struct cache_hits *head,struct inode *inode)
{
	struct cache_hits *temp,*temp1;
	if(f_cnt>10)
		dbg=0;
	if(head==NULL){//if list is empty.
		head=kmalloc(sizeof(struct cache_hits),GFP_KERNEL);
		head->inode=inode->i_ino;
		head->cnt=1;
		head->next=NULL;
		f_cnt++;
		if(dbg==1)
			printk(KERN_INFO "NULL ##PAGE-CACHE-HIT-INFO## INODE: %lu\tCNT: %lu\t\t\t\t",head->inode,head->cnt);
		return head;
	}
	if(head->next==NULL){
		if(head->inode==inode->i_ino)
		{
			(head->cnt)+=1;
			return head;
		}
		temp=kmalloc(sizeof(struct cache_hits),GFP_KERNEL);
		temp->inode=inode->i_ino;
		temp->cnt=1;
		temp->next=NULL;
		head->next=temp;
		f_cnt++;
		if(dbg==1)
			printk(KERN_INFO "## 1st PAGE-CACHE-HIT-INFO## INODE: %lu\tCNT: %lu\t\t\t\t",temp->inode,temp->cnt);
		return head;
	}
	temp=head;
	if(dbg==1)
		printk(KERN_INFO "insertion for INODE: %lu\t\t\t\t",inode->i_ino);
	while(temp->next){
		if(temp->inode==inode->i_ino)
		{
			(temp->cnt)+=1;
			return head;
		}
		temp=temp->next;
		if(dbg==1)
			printk(KERN_INFO "visited INODE: %lu\tCNT: %lu\t\t\t\t",temp->inode,temp->cnt);
	}
	if(dbg)
		printk(KERN_INFO "\n\n");

	temp1=kmalloc(sizeof(struct cache_hits),GFP_KERNEL);
	temp1->inode=inode->i_ino;
	temp1->cnt=1;
	temp1->next=NULL;
	temp->next = temp1;
	f_cnt++;
	return head;
}

void del(struct cache_hits *head){
	struct cache_hits *temp=head;
	if(!head)
		return;
	while(temp->next){
		temp=head->next;
		kfree(head);
		head=temp;
	}
	kfree(temp);
}

void remove_dup(struct cache_hits *head){
	struct cache_hits *temp1=head,*temp2=head,*temp;
	while(temp1){
		temp2=temp1->next;
		while(temp2){
			if(temp1->inode==temp2->inode)
			{
				temp1->cnt+=temp2->cnt;
				temp=temp2->next;
				kfree(temp2);
				temp2=temp;
				continue;
			}
			temp2=temp2->next;
		}
		temp1=temp1->next;
	}
}

void display(struct cache_hits *head){
	struct cache_hits *temp=head;
	while(temp){
		printk(KERN_INFO "#### INODE: %lu\tCNT: %lu\n",temp->inode,temp->cnt);
		temp=temp->next;
	}
}

static void newfunc(struct address_space *m){
	if(m->host && m->host->i_ino)
	{
		write_lock(&i_lock);
		//spin_lock(&m->host->i_lock);
		head=insert(head,m->host);
		//total++;
		//spin_unlock(&m->host->i_lock);
		write_unlock(&i_lock);
	}
}

extern void (*pnt_pagehit)(struct address_space *mapping);
static void (*orig_ref)(struct address_space *mapping);

//extern void (*pnt_pagehit_2)(struct address_space *mapping);
//static void (*orig_ref_2)(struct address_space *mapping);

int init_module(void)
{
//	head=NULL;
    orig_ref    = pnt_pagehit;
    pnt_pagehit = &newfunc;
//    orig_ref_2    = pnt_pagehit_2;
//    pnt_pagehit_2 = &newfunc;
    printk(KERN_INFO "Loading Dummy module\n");
    return 0;
}

void cleanup_module(void)
{
    pnt_pagehit=orig_ref;
//    pnt_pagehit_2=orig_ref_2;
	remove_dup(head);
	display(head);
	del(head);

    //printk(KERN_INFO "Unloading Dummy module. hitcount=%d,files=%u\n",total,f_cnt);
    printk(KERN_INFO "#### MOD-UNLOAD total_files=%u\n",f_cnt);
}

