#include "proto.h"

int kws_worker(void *none)
{
	struct kws_request *request;
	int len;
	INFO("Enter kws_worker\n");

	while(KwsStatus != EXIT) {
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
			request->mem = (char *)kmalloc(request->size, GFP_KERNEL);
			if (request->mem == NULL)
			{
				ERR("Allocate for request->mem failed");
				kws_request_queue_in(RequestQueue, request);
				break;
			}
			request->status = READING;
			/* No break here */
		case READING:
			INFO("case Reading");
			while ((len = kws_sock_read(request->sock,
				request->mem + request->len,
				request->size - request->len - 1)) > 0)
			{
				request->len += len;
			}

			if (len < 0) {
				INFO("Again || Would block || Intrupt");
				while (kws_request_queue_in(RequestQueue, request) < 0)
				{
					wait_event_interruptible(RequestQueue->wq, RequestQueue->count > 0 || KwsStatus == EXIT);
				}
				continue;
			}

			if (len == 0) {
				request->status = DONE;
				request->mem[request->len] = '\0';
				INFO("-----Reading Done-----");
				INFO("%s", request->mem);
				INFO("-----Reading Done-----");
			}

			break;
		default:
			break;
		}
	}

	INFO("Leave kws_worker\n");
	return 0;
}
