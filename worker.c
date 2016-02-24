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

			len = kws_sock_read(request->sock, request->mem + request->len, request->should_read);
			if (len > 0) {
				if (kws_http_parse(request, len) < 0) {

				}

			} else if (len < 0) {
				/* No data available on socket buffer */

			} else { /* len = 0, client socket has been closed. */
				request->mem[request->len] = '\0';
				if (request->header_status = VALIDE) {
					request->status = READINGDONE;
					kws_http_request_handle(request);
				} else {
					request->status = BADREQUEST;
					kws_bad_request_handle(request);
				}
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


