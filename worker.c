#include "proto.h"

int kws_worker(void *none)
{
	struct kws_request *request;
	int len;
	struct kws_string line;
	char *curr;
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

		if (request->status == NEW) {
			INFO("case NEW");
			request->mem = (char *)kmalloc(request->size, GFP_KERNEL);
			if (request->mem == NULL)
			{
				ERR("Allocate for request->mem failed");
				kws_request_queue_in(RequestQueue, request);
				break;
			}
			request->status = READING;
		}

		if (request->status == READING) {
			INFO("case Reading");
			while ((len = kws_sock_read(request->sock,
				request->mem + request->len,
				request->size - request->len - 1)) > 0)
			{
				request->len += len;
			}

			if (len < 0) {
				INFO("Again || Would block || Intrupt");

				for (curr = request->mem + request->old_len ;
					request->bound == 0 && curr + 3 < request->len ;
					curr++, request->old_len++)
				{
					if (curr[0] == '\r'
						&& curr[1] == '\n'
						&& curr[2] == '\r'
						&& curr[3] == '\n')
					{
						request->bound = curr + 3;
						kws_reqeust_parse(request);
					}
					if (request->mem[curr])
				}

				if (request->bound > 0 && request->content_length > 0) {
					if (request->content_length == request->len - request->bound) {
						request->status = DONE;
						/* Complete to handle request
						 * Fork new thread to generate response
						 */
						break;
					}
				}

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
				continue;
			}
		}
	}

	INFO("Leave kws_worker\n");
	return 0;
}
