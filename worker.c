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
				/*
				 * Parse HTTP header from reading request text
				 * @request - parsed request
				 * @read_line - the length of latest reading data
				 * @Return value -
				 *     > 0 : HTTP header parse successfully
				 *     = 0 : HTTP header is incomplete, wait next parse
				 *     < 0 : HTTP header is invalid, this request is bad
				 */
				parse_status = kws_http_parse(request, len);
				if (parse_status == 0) {
					request->len += len;
					request->should_read -= len;
				} else if (parse_status > 0) {
					if (request->connection == KEEPALIVE && request->content_length > 0) {
						request->len += len;
						request->should_read =
							request->content_length
							- (request->len - (request->bound + 2));

					} else if (request->connection == KEEPALIVE && request->content_length == NONE) {
						request->mem[request->bound + 2] = '\0';
					} else {
						/* Read return 0 indicate the end of request */
					}
				} else {
					request->len += len;
					kws_bad_request_handle(request);
				}

			} else if (len < 0) {
				/* No data available on socket buffer */

			} else { /* len = 0, client socket has been closed. */
				request->mem[request->len] = '\0';
				if (request->bound > 0) {
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
