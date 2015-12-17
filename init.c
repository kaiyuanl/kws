#include "proto.h"

MODULE_LICENSE("GPL");

int CPUNUM = 0;

struct socket *Listening_Socket = NULL;

struct task_struct *Master_Thread = NULL;

static int kws_init(void)
{
    struct task_struct *master;

    printk(KERN_DEBUG "Enter kws_init\n");
    printk(KERN_ALERT "Enter Kaiyuan's Web Server\n");

    printk(KERN_ALERT "Create master thread\n");
    master = kthread_run(&kws_master_init, NULL, "KWS Master Thread");
    if(master == NULL)
    {
        printk(KERN_ERR "Create master thread FAILED\n");
    }
    Master_Thread = master;

    printk(KERN_DEBUG "Leave kws_init\n");

    return 0;
}

static void kws_exit(void)
{
    printk(KERN_DEBUG "Enter kws_exit\n");
	printk(KERN_ALERT "Leave Kaiyuan's Web Server\n");
    if(Master_Thread)
    {
        if(kthread_stop(Master_Thread) < 0)
        {
            printk(KERN_ERR "Release master thread FAILED\n");
        }
    }
    printk(KERN_DEBUG "Leave kws_exit\n");
}

module_init(kws_init);
module_exit(kws_exit);
