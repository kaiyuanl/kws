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
#include <linux/jiffies.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <net/sock.h>

#define BACKLOG 64
#define REQ_QUEUE_SIZE 1024
#define REQ_MEM_SIZE 2048
#define HASHBKT 23

#define OLD -1		/* Lagency structure, will be reclaimed to cache */
#define NEW 0		/* New connection */
#define READING 1	/* Reading data from socket*/
#define READDONE 2	/* Finish read data from socket*/
#define REUSE 3		/* HTTP connection is keep-alive */
#define REUSEDONE 4	/* Finish read a request for reuse connection*/
#define BADREQUEST 5	/* Bad HTTP request*/
#define REQTIMEOUT 6
#define REUSECLOSE 7

#define DEFAULT_WWWROOT		"/var/wwwroot"

#define INFO(x, ...)  printk(KERN_DEBUG x"\n", ##__VA_ARGS__)

#ifdef KWS_DEBUG
#define ERR(x,...)  printk(KERN_ALERT x"at %s line %i\n", ##__VA_ARGS__ ,__FILE__,__LINE__)
#else
#define ERR(x,...)  do {} while (0)
#endif

#define RUNNING 0
#define RESTART	1
#define STOP	2
#define EXIT	3

#define NONE -1
#define NONDIGIT -1
#define INTOVERFLOW -1
#define NOINTSTR -2

#define TIMEOUT -1
#define TIMEIN 0
#define REQUEST_TIMEOUT (HZ * 1 / 200)

/* String struct in kws.
 * pstart != NULL && len > 0:		valid string
 * pstart != NULL && len == 0:		zero-length string
 * pstart == NULL && len == 0:		null
 * other case:				invalid string
 */
typedef struct kws_string {
	char *pstart;
	size_t len;
} kws_string;

#define INVALID_STR(str) (!((str).pstart) || ((str).len <= 0))
#define NULL_STR  { .pstart = NULL, .len = -1 };

typedef struct kws_field_kv {
	/*If line doesn't contain http field key-value, set errno to -1*/
	int errno;
	kws_string key;
	kws_string value;
	struct list_head list;
} kws_field_kv;

typedef struct kws_request {
	struct socket *sock;
	unsigned long create_time;
	int method;
	int http_version;
	int status;
	char *mem;
	int size;
	int len;
	int should_read;
	int bound;

	int connection;
	int content_length;

	int status_code;
	char *status_msg;
	struct kws_string url;
	struct kws_field_kv fields;
} kws_request;

struct kws_queue {
	spinlock_t lock;
	wait_queue_head_t wq;
	unsigned int size;
	unsigned int in;
	unsigned int out;
	unsigned int count;
	void **items;
};

struct kws_pool_task {
	struct task_struct *task;
	void (*task_handler)(void *data);
	void *task_param;
	int status;
	struct list_head list;
};

#define INIT		0x00
#define NEWTASK		0x01
#define TASKDONE	0x02

struct kws_pool {
	spinlock_t lock;
	struct kws_pool_task tasks;
	int size;
};

extern int KwsStatus;

extern int CPU;
extern int ListeningPort;
extern struct socket *ListeningSocket;
extern struct task_struct *Master;
extern struct task_struct **Workers;
extern struct task_struct *Pooler;
extern int WorkerNum;
extern int PoolThreadNum;
extern size_t MemSize;
extern char *WWWRoot;

extern struct kws_queue *RequestQueue;
extern struct kws_queue *DoneRequestQueue;
extern struct kws_pool *ThreadPool;

extern void (*kws_task_handler)(void *data);

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
void kws_bad_request_handle(struct kws_request *request);
void kws_http_request_handle(void *data);
int kws_request_timeout(struct kws_request *request);


int kws_strstr(char *s1, int l1, char *s2, int l2);
int kws_http_parse(struct kws_request *request, size_t read_len);

int kws_task_pool_init(struct kws_pool **pool, size_t size);
int kws_pooler(void *none);

void kws_default_handle(void *data);

void kws_send_200_header(struct socket *sock);

#endif
