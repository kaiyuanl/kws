#include "proto.h"

int kws_worker(void *none)
{
	struct kws_request *request;
	int len;
	printk(KERN_DEBUG "Enter kws_worker\n");

	while(1) {
		if (RequestQueue == NULL)
			return 0;

		wait_event_interruptible(RequestQueue->wq, RequestQueue->count > 0 || KwsStatus == EXIT);
		if (KwsStatus == EXIT) {
			return 0;
		}

		request = kws_request_queue_out(RequestQueue);
		if (request == NULL) {
			INFO("Request queue empty");
			continue;
		}

		switch (request->status) {
		case NEW:
			INFO("case NEW");
			request->mem = (char *)kmalloc(PAGE_SIZE, GFP_KERNEL);
			if (request->mem == NULL)
			{

			}
			request->status = READING;
			/* No break here */
		case READING:
			INFO("case Reading");
			while ((len = kws_sock_read(request->sock,
				request->mem + request->len,
				request->size - request->len)) >= 0)
			{
				request->len += len;
			}

			if (len == EAGAIN || len == EWOULDBLOCK || len == EINTR) {
				kws_request_queue_in(RequestQueue, request);
				continue;
			}

			if (len == 0) {
				request->status = DONE;
				request->mem[len] = '\0';
				printk(KERN_DEBUG "%s\n", request->mem);
			}

			break;
		case DONE:
			printk(KERN_INFO "%s", request->mem);
			break;
		case OLD:
			break;
		default:
			break;
		}
	}

	printk(KERN_DEBUG "Leave kws_worker\n");
	return 0;
}
