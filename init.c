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

int CPU;
int WorkerNum;
int ListeningPort;
struct socket *ListeningSocket = NULL;
struct task_struct *Master = NULL;


static int kws_init(void)
{
	struct task_struct *master;

	INFO("---------------Start Web Server---------------\n");
	INFO("Enter kws_init");

	ListeningPort = port;
	WorkerNum = workers;
	master = kthread_run(&kws_master, NULL, "kws master thread");
	if(master == NULL) {
		ERR("Create master thread failed\n");
	}
	Master = master;

	INFO("Leave kws_init");

	return 0;
}

static void kws_exit(void)
{
	int i;
	INFO("Enter kws_exit");

	for (i = 0; i < WorkerNum; i++) {
		if (Workers[i] == NULL)
			continue;

		if (kthread_stop(Workers[i]) < 0) {
			ERR("Stop worker thread %d failed", i);
		}
	}

	if (Master != NULL && kthread_stop(Master) < 0) {
		ERR("Stop master thread failed");
	}

	kws_request_queue_release(RequestQueue);

	kws_sock_release(ListeningSocket);
	INFO("Leave kws_exit");
}

module_init(kws_init);
module_exit(kws_exit);
