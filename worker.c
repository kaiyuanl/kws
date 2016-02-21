#include "proto.h"

int kws_worker(void *none)
{
	struct kws_request *request;
	int len;
	int pos;

	INFO("Enter kws_worker\n");

	while(1) {
		if (RequestQueue == NULL) {
			return 0;
		}

		len = 0;
		pos = 0;

		wait_event_interruptible(RequestQueue->wq,
			RequestQueue->count > 0 || KwsStatus == EXIT);

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
				continue;
			}
			request->status = READING;
			request->should_read = request->size - 1;
		}

		if (request->status == READING) {
			INFO("Reading on socket");
			if (kws_request_timeout(request) == TIMEOUT) {
				request->status = REQTIMEOUT;
				kws_bad_request_handle(request);
				continue;
			}

			while ((len = kws_sock_read(request->sock,
				request->mem + request->len,
				request->should_read)) > 0)
			{
				INFO("Inside loop, len = %d", len);
				request->old_len = request->len;
				request->len += len;
				request->should_read -= len;
			}

			INFO("Outside loop, len = %d", len);
			INFO("request->len = %d, old_len = %d", request->len, request->old_len);

			if (len == 0) {
				/* Client has closed this connection */
				request->mem[request->len] = '\0';
				request->status = READDONE;
				kws_http_request_handle(request);
				continue;
			}

			if (request->bound == 0) {
				INFO("Start find double CRLFs");
				pos = kws_strstr(request->mem + (request->old_len >= 4 ? request->old_len - 4 : 0),
					request->len - (request->old_len - 4),
					"\r\n\r\n",
					4);

				if (pos >= 0) {
					INFO("Get double CRLFs in request");

					request->bound = request->old_len + pos;

					if (kws_http_parse(request) < 0) {
						ERR("Request parse failed");
						request->status = BADREQUEST;
						kws_bad_request_handle(request);
						continue;
					}

					INFO("Bound = %d\n", (int)request->bound);
					switch (request->parser->header_state) {
					case h_connection_keep_alive:
						request->status = REUSEDONE;
						request->mem[request->len] = '\0';
						break;
					case h_content_length:
						request->status = REUSE;
						request->should_read
							= request->parser->content_length
								- (request->len - request->bound);

						break;
					case h_general:
						/* Do Nothing */
						break;
					default:
						break;
					}
				}
			}

			if (request->should_read <= 0) {
				ERR("Request too large");
				continue;
			}
		}

		if (request->status == REUSE) {
			if (kws_request_timeout(request) == TIMEOUT) {
				request->status = REQTIMEOUT;
				kws_bad_request_handle(request);
				continue;
			}
			while	(
						likely(request->should_read > 0)
						&&
						(len = kws_sock_read(request->sock,
							request->mem + request->len,
							request->should_read) > 0)
					)
			{
				request->old_len = request->len;
				request->len += len;
				request->should_read -= len;
			}

			if (len == 0) {
				/* Client close socket */
				request->status = REUSECLOSE;
				kws_bad_request_handle(request);
				continue;
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
				kws_http_request_handle(request);
			}
		}

		if (request->status == REUSEDONE) {
			if (kws_request_timeout(request) == TIMEOUT) {
				request->status = REQTIMEOUT;
				kws_bad_request_handle(request);
				continue;
			}
		}

		while (kws_request_queue_in(RequestQueue, request) < 0) {
			wait_event_interruptible(RequestQueue->wq, RequestQueue-> count < RequestQueue->size || KwsStatus == EXIT);
			if (KwsStatus == EXIT) {
				return 0;
			}
		}
	}

	INFO("Leave kws_worker\n");
	return 0;
}


