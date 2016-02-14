#include "proto.h"

int kws_worker(void *none)
{
	struct kws_request *request;
	size_t len;
	int pos;
	enum header_states state;
	INFO("Enter kws_worker\n");
	state = h_general;
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

		/* In coming new connection */
		if (request->status == NEW) {
			INFO("New Connection");
			request->mem = (char *)kmalloc(request->size, GFP_KERNEL);
			if (request->mem == NULL)
			{
				ERR("Allocate for request->mem failed");
				kws_request_queue_in(RequestQueue, request);
				break;
			}
			request->status = READING;
			request->should_read = request->size - 1;
		}

		if (request->status == READING) {
			INFO("Reading on socket");
			while ((len = kws_sock_read(request->sock, request->mem + request->len, request->should_read)) > 0) {
				request->old_len = request->len;
				request->len += len;
				request->should_read -= len;
			}

			if (len == 0) {
				/* Client has closed this connection */
				request->mem[request->len] = '\0';
				//
				INFO("%s", request->mem);
				continue;
			}

			pos = kws_strstr(request->mem + request->old_len - 4, request->len + 4, "\r\n\r\n", 4);
			if (pos >= 0) {
				request->bound = request->old_len - 4 + pos;
				kws_http_parse(request);
				INFO("Bound = %d\n", (int)request->bound);
			}

			/*
			if (request->parser->header_state == 10) {
				request->status = REUSE;
				if (request->parser->content_length <= 0) {
					request->mem[request->bound] = '\0';
					INFO("%s", request->mem);
				}

				if (request->parser->content_length > 0) {
					request->should_read = request->parser->content_length - request->len + request->bound;
					if (request->should_read <= 0) {
						request->mem[request->bound + request->parser->content_length + 4] = '\0';
						INFO("%s", request->mem);
					}
				}

				while (kws_request_queue_in(RequestQueue, request) < 0) {
					wait_event_interruptible(RequestQueue->wq, RequestQueue->count < RequestQueue->size || KwsStatus == EXIT);
					if (KwsStatus == EXIT) {
						return 0;
					}
				}
			} else if (request->parser->header_state = header_states.)
			*/

			switch (request->parser->header_state) {
			case h_connection_keep_alive:
				break;
			case h_content_length:
				break;
			case h_general:
				break;
			default:
				break;
			}


			if (request->should_read == 0) {
				ERR("Out of Memory");
				continue;
			}

			while (kws_request_queue_in(RequestQueue, request) < 0) {
				wait_event_interruptible(RequestQueue->wq, RequestQueue-> count < RequestQueue->size || KwsStatus == EXIT);
				if (KwsStatus == EXIT) {
					return 0;
				}
			}

		}

		if (request->status == REUSE) {
			while (likely(request->should_read > 0) &&
				(len = kws_sock_read(request->sock, request->mem + request->len, request->should_read) > 0))
			{
				request->old_len = request->len;
				request->len += len;
				request->should_read -= len;
			}

			if (len < 0) {

			}

			if (request->should_read == 0) {
				/* Server has completed reading current request
				 * Set status of this request 'Reuse Pending'.
				 * Worker don't deal with requests in 'Reuse Pending'
				 * until response generator cleanups and set request
				 * back to 'Reading' status.
				 */
				request->mem[request->len] = '\0';
				request->status = REUSEDONE;
				//kws_http_handle(request);
			}

			while (kws_request_queue_in(RequestQueue, request) < 0) {
				wait_event_interruptible(RequestQueue->wq, RequestQueue->count < RequestQueue->size || KwsStatus == EXIT);
				if (KwsStatus == EXIT) {
					return 0;
				}
			}
		}

		if (request->status == REUSEDONE) {
			if (kws_request_timeout(request) < 0) {
				continue;
			}
			while (kws_request_queue_in(RequestQueue, request) < 0) {
				wait_event_interruptible(RequestQueue->wq, RequestQueue->count < RequestQueue->size || KwsStatus == EXIT);
				if (KwsStatus == EXIT) {
					return 0;
				}
			}
		}
	}

	INFO("Leave kws_worker\n");
	return 0;
}


