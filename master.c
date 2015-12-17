#include "proto.h"

int Port = 8080;

int kws_master_init(void *none)
{
    struct socket *new_sock = NULL;
    int r;
    printk(KERN_DEBUG "Enter master_init\n");

    CPUNUM = num_online_cpus();
    printk(KERN_DEBUG "CPUNUM = %d\n", CPUNUM);

    if(kws_start_listen(Port) < 0)
    {
        printk(KERN_ERR "Start listen FAILED\n");
        return -1;
    }

    r = kws_epoll_create(&Eventpoll);
    printk(KERN_DEBUG "Epoll_create return %d\n", r);

    if(kws_accept(&new_sock) < 0)
    {
        printk(KERN_ERR "Accept FAILED\n");
        return -1;
    }

    printk(KERN_DEBUG "Leave master_init\n");
    return 0;
}
