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
	request->size = 1024;
	request->len = 0;

	request->lstart = 0;
	request->lpos = 0;
	request->status = OUTLINE;

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

static kws_string getline(char *string, int start, int end)
{
	char c;
	char *p;
	p = string + start;
}

void kws_request_parse(struct kws_request *request)
{
	char *pos;
	kws_string line;
	INFO("Enter kws_request_parse");
	pos = request->mem;
	while ()
	INFO("Leave kws_request_parse");
}
