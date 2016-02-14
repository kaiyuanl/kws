#include "proto.h"

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
	request->old_len = 0;
	request->should_read = 0;

	request->parser = (http_parser *)kmalloc(sizeof(http_parser), GFP_KERNEL);

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

	INFO("Release HTTP parser");
	kfree(request->parser);

	INFO("Release request");
	kfree(request);
	INFO("Leave kws_request_release");
}

void kws_http_request_handle(struct kws_request *request)
{

}

int kws_request_timeout(struct kws_request *request)
{
	return 0;
}

