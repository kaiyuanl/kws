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

	INFO("Enter kws_request_init");
	request = (struct kws_request *)kmalloc(sizeof(struct kws_request), GFP_KERNEL);
	if (request) {
		ERR("Allcoate memory for reqeust struct FAILED");
		return NULL;
	}
	request->sock = NULL;
	request->status = NEW;
	request->mem = NULL;
	request->size = PAGE_SIZE;
	request->len = 0;

	INFO("Leave kws_request_init");
	return request;
}

void kws_request_release(struct kws_request *request)
{
	if (request != NULL) {

		if (request->sock != NULL)
			kws_sock_release(request->sock);

		kfree(request->mem);
		kfree(request);
	}
}
