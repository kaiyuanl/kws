#ifndef _PROTO_H_
#define _PROTO_H_

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/net.h>
#include <linux/cpumask.h>
#include <linux/syscalls.h>
#include <linux/eventpoll.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/poll.h>
#include <linux/spinlock.h>
#include <linux/rbtree.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <net/sock.h>

#define BACKLOG 48

extern int CPUNUM;

extern int Port;

extern struct socket *Listening_Socket;

extern struct task_struct *Master_Thread;

extern struct eventpoll *Eventpoll;

struct kws_http_request{

};

struct kws_wait_queue{

};

asmlinkage long sys_epoll_create1(int flags);

int kws_master_init(void *none);

int kws_worker_init(void *none);

struct socket *kws_sock_alloc(void);

int kws_sock_release(struct socket *sk);

int kws_start_listen(int port);

int kws_epoll_create(struct eventpoll **new_ep);

int kws_epoll_ctl(void);

int kws_accept(struct socket **new_sock);

#endif
