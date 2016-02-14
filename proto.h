#ifndef _PROTO_H_
#define _PROTO_H_

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/spinlock.h>
#include <linux/cpumask.h>
#include <linux/hashtable.h>
#include <net/sock.h>

#include "http_parser.h"

#define BACKLOG 64
#define REQ_QUEUE_SIZE 1024
#define REQ_MEM_SIZE 2048
#define HASHBKT 23

#define OLD -1		/* Lagency structure, will be reclaimed to cache */
#define NEW 0		/* New connection */
#define READING 1	/* Reading data from socket*/
#define DONE 2		/* Finish read data from socket */
#define REUSE 3
#define REUSEDONE 4
#define BADREQUEST 5

#define INLINE 0
#define OUTLINE 1

#define INFO(x, ...)     printk(KERN_ALERT x"\n", ##__VA_ARGS__)

#ifdef KWS_DEBUG
#define ERR(x,...)      printk(KERN_ALERT x"at %s line %i\n", ##__VA_ARGS__ ,__FILE__,__LINE__)
#else
#define ERR(x,...)      do {} while (0)
#endif

extern int KwsStatus;
#define RUNNING 0
#define RESTART	1
#define STOP	2
#define EXIT	3

/* HTTP methods */
#define NOSUPPORT 0
#define GET 1
#define POST 2
#define PUT 3
#define DELETE 4


extern int CPU;
extern int ListeningPort;
extern struct socket *ListeningSocket;
extern struct task_struct *Master;
extern struct task_struct **Workers;
extern int WorkerNum;
extern size_t MemSize;

struct kws_string {
	char *pstart;
	size_t len;
};

struct kws_request {
	struct socket *sock;
	int status;
	char *mem;
	size_t size;
	size_t old_len;
	size_t len;
	size_t should_read;
	size_t bound;

	http_parser *parser;
};

struct kws_queue {
	spinlock_t lock;
	wait_queue_head_t wq;
	unsigned int size;
	unsigned int in;
	unsigned int out;
	unsigned int count;
	void **items;
};

extern struct kws_queue *RequestQueue;

int kws_master(void *none);
int kws_worker(void *none);


struct socket *kws_sock_alloc(void);
void kws_sock_release(struct socket *sock);
struct socket *kws_sock_listen(int port);
struct socket *kws_accept(struct socket *sock);
int kws_sock_read(struct socket *sock, char *buff, size_t len);
int kws_sock_write(struct socket *sock, char *buff, size_t len);


struct kws_queue *kws_queue_alloc(unsigned int size);
int kws_queue_in(struct kws_queue *queue, void *item);
void *kws_queue_out(struct kws_queue *queue);
void kws_queue_release(struct kws_queue *queue);

struct kws_queue *kws_request_queue_alloc(unsigned int size);
int kws_request_queue_in(struct kws_queue *queue, struct kws_request *request);
struct kws_request *kws_request_queue_out(struct kws_queue *queue);
void kws_request_queue_release(struct kws_queue *queue);


struct kws_request *kws_request_alloc(void);
void kws_request_release(struct kws_request *request);
int kws_request_timeout(struct kws_request *request);

unsigned int kws_hash(struct kws_string str);
void kws_hash_add(struct hlist_head fields[], int bkt, struct kws_string field);
struct kws_string kws_hash_get(struct hlist_head fields[], int bkt, char *field);

int kws_strstr(char *s1, int l1, char *s2, int l2);
int kws_http_parse(struct kws_request *request);

#endif
