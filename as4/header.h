struct my_rq {
        spinlock_t          lock;   /* spin lock that protects this runqueue */
        unsigned long       nr_running;         /* number of runnable tasks */
        unsigned long       nr_switches;        /* context switch count */
        unsigned long       expired_timestamp;    /* time of last array swap */
        unsigned long       nr_uninterruptible;   /* uninterruptible tasks */
        unsigned long long  timestamp_last_tick;  /* last scheduler tick */
        struct task_struct  *curr;                /* currently running task */
        struct task_struct  *idle;           /* this processor's idle task */
        struct mm_struct    *prev_mm;        /* mm_struct of last ran task */
        struct task_struct  *migration_thread; /* migration thread */
        struct list_head    migration_queue;   /* migration queue*/
        atomic_t            nr_iowait; /* number of tasks waiting on I/O */
	int                 cpu;	//cpu id
	unsigned long long  now;	//cpu's time
};

