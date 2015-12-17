#include "proto.h"

struct socket *kws_sock_alloc(void)
{
    struct socket *new_sock = NULL;

    printk(KERN_DEBUG "Enter kws_sock_alloc\n");

    if(sock_create_lite(PF_INET,SOCK_STREAM,IPPROTO_TCP, &new_sock) < 0)
    {
        return NULL;
    }

    printk(KERN_DEBUG "Leave kws_sock_alloc\n");

    return new_sock;
}

int kws_sock_release(struct socket *sk)
{
    return 0;
}

int kws_start_listen(int port)
{
    struct socket *sock;
    struct sockaddr_in sin;
    int error;

    printk(KERN_DEBUG "Enter kws_start_listen\n");

    error = sock_create(PF_INET,SOCK_STREAM,IPPROTO_TCP,&sock);
    if (error < 0)
    {
         printk(KERN_ERR "Create socket FAILED\n");
         return -1;
    }

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons((unsigned short)port);

    printk(KERN_DEBUG "Bind socket to port %d\n", port);
    error = sock->ops->bind(sock,(struct sockaddr*)&sin,sizeof(sin));
    if (error < 0)
    {
        printk(KERN_ERR "Bind socket FAILED\n");
        return -1;
    }

    //sock->sk->reuse = SK_CAN_REUSE;

    error=sock->ops->listen(sock,BACKLOG);
    if (error < 0)
    {
        printk(KERN_ERR "Listen socket FAILED\n");
        return -1;
    }

    Listening_Socket = sock;

    printk(KERN_DEBUG "Leave kws_start_listen\n");
    return 0;
}

int kws_epoll_create(struct eventpoll **new_ep)
{
    return sys_epoll_create1(1);
}

int kws_epoll_ctl(void)
{
    return 0;
}

int kws_accept(struct socket **new_sock)
{
    int error;

    printk(KERN_DEBUG "Enter kws_accept\n");


    if (Listening_Socket == NULL)
    {
        return -1;
    }

    *new_sock = kws_sock_alloc();
    if (new_sock == NULL|| *new_sock == NULL)
    {
        printk(KERN_ERR "Allocate socket FAILED\n");
        return -1;
    }

    (*new_sock)->ops = Listening_Socket->ops;

    printk(KERN_DEBUG "Start accept\n");
    error = Listening_Socket->ops->accept(Listening_Socket,*new_sock,O_RDWR);
    printk(KERN_DEBUG "Finish accept\n");

    if(error >= 0 && *new_sock)
    {
        printk(KERN_DEBUG "Accept Completed\n");
    }

    printk(KERN_DEBUG "Leave kws_accept\n");
    return error;
}
