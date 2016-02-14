#include "proto.h"

int kws_strstr(char *s1, int l1, char *s2, int l2)
{
	int index;
	if (l1 < l2)
		return -1;
	if (s1 == NULL || s2 == NULL)
		return -1;
	for (index = 0; index <= l1 - l2; index++) {
		if (!memcmp(s1+index, s2, l2))
			return index;
	}
	return -1;
}

int kws_http_parse(struct kws_request *request)
{
	size_t nparsed;
	http_parser_settings settings;
	http_parser_init(request->parser, HTTP_REQUEST);
	nparsed = http_parser_execute(request->parser, &settings, request->mem, request->len);

	if (nparsed != request->len) {
		//ERR("HTTP request parse failed");
		request->status = BADREQUEST;
		return -1;
	}

	return 0;
}
