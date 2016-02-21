#include "proto.h"

struct kws_queue *RequestQueue;
struct task_struct **Workers;

int kws_master(void *none)
{
	struct socket *sock;
	struct socket *new_sock;
	struct kws_request *request;
	int i;

	INFO("Enter kws_master");

	CPU = num_online_cpus();

	if (WorkerNum == 0)
		WorkerNum = CPU;

	sock = kws_sock_listen(ListeningPort);
	if (sock == NULL) {
		ERR("Listen failed");
		return -1;
	}

	ListeningSocket = sock;

	RequestQueue = kws_request_queue_alloc(REQ_QUEUE_SIZE);
	if (RequestQueue == NULL) {
		return -1;
	}

	Workers = (struct task_struct **)kmalloc(WorkerNum * sizeof(struct task_struct *), GFP_KERNEL);
	if (Workers == NULL)
		return -1;

	for (i = 0; i < WorkerNum; i++) {
		Workers[i] = kthread_create(&kws_worker, NULL, "kws worker thread %d", i);

		if (!IS_ERR(Workers[i])) {
			kthread_bind(Workers[i], i);
			wake_up_process(Workers[i]);
			INFO("Create thread worker %d", i);
		} else {
			ERR("Create worker thread failed");
		}
	}

	while(KwsStatus != EXIT) {

		new_sock = kws_accept(ListeningSocket);

		if (new_sock == NULL)
			continue;

		request = kws_request_alloc();
		if (request == NULL) {
			INFO("Allocate for request failed");
			kws_sock_release(new_sock);
			continue;
		}

		request->sock = new_sock;
		request->create_time = jiffies;

		while (kws_request_queue_in(RequestQueue, request) < 0) {
			if (KwsStatus == EXIT) {
				return 0;
			}
			wake_up_interruptible(&(RequestQueue->wq));
		}

		INFO("Finish enqueue");
		wake_up_interruptible(&(RequestQueue->wq));
	}

	INFO("Leave master_init\n");
	return 0;
}
