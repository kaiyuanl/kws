#include "proto.h"

MODULE_AUTHOR("Kaiyuan Liang");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel-based Web Server");

static int port = 8080;
module_param(port, int, 0644);
MODULE_PARM_DESC(port, "The port kws will listen");

static int workers = 0;
module_param(workers, int, 0644);
MODULE_PARM_DESC(workers, "");

static int status = RUNNING;
module_param(status, int, 0644);
MODULE_PARM_DESC(status, "Running - 0\nRestart - 1\nStop - 2");

int CPU;
int WorkerNum;
int KwsStatus;
int ListeningPort;
struct socket *ListeningSocket;
struct task_struct *Master;
size_t MemSize;
struct kws_pool *ThreadPool;
void (*kws_task_handler)(void *data);


static int kws_init(void)
{
	struct task_struct *master;

	INFO("---------------Start Web Server---------------\n");
	INFO("Enter kws_init");

	ListeningPort = port;
	WorkerNum = workers;
	KwsStatus = status;
	MemSize = PAGE_SIZE;
	master = kthread_run(&kws_master, NULL, "kws master thread");
	if(master == NULL) {
		ERR("Create master thread failed\n");
	}
	Master = master;
	kws_task_handler = &kws_http_request_handle;
	INFO("Leave kws_init");

	return 0;
}

static void kws_exit(void)
{
	int i;
	INFO("Enter kws_exit");

	INFO("Release worker threads");
	KwsStatus = EXIT;
	if (Workers != NULL) {
		if (RequestQueue != NULL) {
			INFO("Wakeup worker threads so that they can finish themselves");
			wake_up_interruptible(&(RequestQueue->wq));
		}

		for (i = 0; i < WorkerNum; i++) {
			INFO("Release worker");

			if (IS_ERR(Workers[i])) {
				continue;
			}

			INFO("Worker %p\n", Workers[i]);
			if (kthread_stop(Workers[i]) < 0) {
				ERR("Stop worker thread failed");
			}
			INFO("Finish release worker");
		}
	}

	INFO("Release master thread");
	if (Master != NULL && kthread_stop(Master) < 0) {
		ERR("Stop master thread failed");
	}

	INFO("Release request queue");
	kws_request_queue_release(RequestQueue);

	INFO("Release listening socket");
	kws_sock_release(ListeningSocket);
	INFO("Leave kws_exit");
	INFO("---------------Exit Web Server---------------\n");
}

module_init(kws_init);
module_exit(kws_exit);
