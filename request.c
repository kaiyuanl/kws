#include "proto.h"
#include "http.h"
void kws_request_cleanup(struct kws_request *request)
{
	if (request == NULL)
		return;

	if (request->sock != NULL) {
		kws_sock_release(request->sock);
		request->sock = NULL;
	}
	request->status = REUSE;
	request->len = 0;
}

struct kws_request *kws_request_alloc(void)
{
	struct kws_request *request;

	INFO("Enter kws_request_alloc");
	request = (struct kws_request *)kmalloc(sizeof(struct kws_request), GFP_KERNEL);
	if (request == NULL) {
		ERR("Allcoate memory for reqeust struct FAILED");
		return NULL;
	}
	request->sock = NULL;
	request->status = NEW;
	request->mem = NULL;
	request->size = REQ_MEM_SIZE;
	request->len = 0;
	request->bound = 0;
	request->should_read = 0;
	request->connection = KEEPALIVE;
	request->content_length = NONE;

	request->mem = (char *)kmalloc(request->size, GFP_KERNEL);
	if (request->mem == NULL)
	{
		ERR("Allocate for request->mem failed");
		kws_request_queue_in(RequestQueue, request);
		kfree(request);
		return NULL;
	}

	INFO("Leave kws_request_alloc");
	return request;
}

void kws_request_release(struct kws_request *request)
{
	INFO("Enter kws_request_release");
	if (request == NULL)
		return;

	INFO("Release socket");
	if (request->sock != NULL)
		kws_sock_release(request->sock);

	INFO("Release mem");
	kfree(request->mem);


	INFO("Release request");
	kfree(request);
	INFO("Leave kws_request_release");
}

void kws_bad_request_handle(struct kws_request *request)
{
	switch(request->status) {
	case REQTIMEOUT:
		break;
	case BADREQUEST:
		break;
	case REUSECLOSE:
		break;
	default:
		break;
	}
	kws_request_release(request);
}

int kws_request_timeout(struct kws_request *request)
{
	if (request == NULL) {
		ERR("Parameter is NULL");
		return TIMEOUT;
	}
	if (request->create_time < jiffies - REQUEST_TIMEOUT) {
		INFO("Request timeout");
		return TIMEOUT;
	} else {
		return TIMEIN;
	}
}

void kws_http_request_handle(void *data)
{
	struct kws_request *request;
	if (data == NULL) {
		ERR("Parameter is NULL");
		return;
	}
	request = (struct kws_request *)data;
	INFO("-----HTTP request-----");
	INFO("%s", request->mem);
	INFO("----------------------");
	INFO("Handle this request in thread pool");
}
