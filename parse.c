#include "proto.h"
#include "http.h"

kws_string kws_trim(kws_string str)
{
	char *p, *q;

	if (str.pstart == NULL || str.len == 0) {
		return str;
	}

	p = str.pstart;
	q = str.pstart + str.len - 1;

	while (*p == ' ' && p < q) {
		p++;
	}

	while ((*q == ' ' || *q == '\r') && p < q) {
		q--;
	}

	str.pstart = p;
	str.len = q - p + 1;
	return str;
}

kws_string kws_getline(char *str,int *pos, int len)
{
	char *p, *q, *end;
	kws_string line;

	if (str == NULL || len <= 0 || *pos >= len) {
		line.pstart = NULL;
		line.len = 0;
		return line;
	}

	p = q = str + *pos;
	end = str + len;
	while (q < end) {
		if (*q == '\n')
			break;
		else
			q++;
	}

	if (q >= end) {
		line.pstart = NULL;
		line.len = 0;
	}

	*pos = q - str + 1;

	line.pstart = p;
	line.len = q - p;
	line = kws_trim(line);
	return line;
}

kws_field_kv kws_str_kv(kws_string line)
{
	char *p, *q, *end;
	kws_string key, value;
	kws_field_kv kv;

	if (line.pstart == NULL || line.len <= 3) {
		kv.errno = -1;
		return kv;
	}

	p = q = line.pstart;
	end = line.pstart + line.len;
	while (q < end && *q != ':') {
		q++;
	}

	if (q >= end) {
		kv.errno = -1;
		return kv;
	}

	key.pstart = p;
	key.len = q - p;

	p = q = q + 1;
	while (q < end) {
		q++;
	}

	value.pstart = p;
	value.len = q - p;

	key = kws_trim(key);
	value = kws_trim(value);

	if (key.len == 0 || value.len == 0) {
		kv.errno = -1;
		return kv;
	}

	kv.key = key;
	kv.value = value;
	kv.errno = 0;
	return kv;
}

int kws_http_method(kws_string *line)
{
	if ((*line).len < 4) {
		return -1;
	}

	if (strncmp((*line).pstart, "GET ", 4) == 0) {
		(*line).pstart += 4;
		(*line).len -= 4;
		return 1;
	}
	return -1;
}

kws_string kws_http_url(kws_string *line)
{
	char *p, *q, *end;
	kws_string url;
	if (INVALID_STR(*line)) {
		url.pstart = NULL;
		url.len = 0;
		return url;
	}

	p = (*line).pstart;
	end = (*line).pstart + (*line).len;
	while (p < end && *p == ' ') {
		p++;
	}

	if (p >= end) {
		url.pstart = NULL;
		url.len = 0;
		return url;
	}

	q = p;
	while (q < end && *q != ' ') {
		q++;
	}

	if (q >= end) {
		url.pstart = NULL;
		url.len = 0;
		return url;
	}

	url.pstart = p;
	url.len = q - p;
	(*line).pstart = q + 1;
	(*line).len = end - (*line).pstart;
	return url;
}

int kws_http_version(kws_string line)
{
	line = kws_trim(line);

	if (INVALID_STR(line) || line.len < 8) {
		return -1;
	}

	if (strncmp(line.pstart, "HTTP/1.1", 8) == 0) {
		return 11;
	}

	if (strncmp(line.pstart, "HTTP/1.0", 8) == 0) {
		return 10;
	}

	if (strncmp(line.pstart, "HTTP/0.9", 8) == 0) {
		return 9;
	}

	return -1;
}

int kws_strcmp(kws_string field, char *str, int len)
{
	int i;
	if (field.len != len) {
		return -1;
	}

	for (i = 0; i < len; i++) {
		if (field.pstart[i] != str[i]) {
			return -1;
		}
	}
	return 0;
}

void kws_str_print(kws_string str)
{
	int i;
	for (i = 0; i < str.len; i++) {
		printk("%c", str.pstart[i]);
	}
}

int kws_header_parse(struct kws_request *request)
{
	int pos = 0;
	kws_string line, url;
	kws_field_kv *kv;
	int method;
	int version;

	INIT_LIST_HEAD(&(request->fields.list));

	line = kws_getline(request->mem, &pos, request->bound);
	if (line.pstart == NULL && line.len <= 0) {
			return -1;
	}

	method = kws_http_method(&line);
	switch (method) {
		case GET:
			request->method = GET;
			break;
		case POST:
		case PUT:
		case DELETE:
		default:
			request->method = method;
			request->status = BADREQUEST;
			request->status_code = 405;
			request->status_msg = MSG405;
			return -1;
	}

	url = kws_http_url(&line);
	if (INVALID_STR(url)) {
		request->status = BADREQUEST;
		request->status_msg = "Request URL invalid";
		return -1;
	}

	request->url = url;

	version = kws_http_version(line);

	if (version < 0) {
		request->status = BADREQUEST;
		request->status_code = 505;
		request->status_msg = MSG505;
		return -1;
	}

	request->http_version = version;

	while(1) {
		line = kws_getline(request->mem, &pos, request->bound);
		if (line.pstart == NULL && line.len <= 0) {
			break;
		}

		kv = (kws_field_kv *)kmalloc(sizeof(kws_field_kv), GFP_KERNEL);
		*kv = kws_str_kv(line);
		if (kv->errno == -1) {
			request->status = BADREQUEST;
			request->status_code = 400;
			request->status_msg = MSG400;
			return -1;
		}

		INIT_LIST_HEAD(&(kv->list));
		list_add_tail(&(kv->list), &(request->fields.list));
	}

	return 0;
}

int kws_str_search(char *str1, size_t len1, char *str2, size_t len2)
{
	int index;
	if (len1 < len2)
		return -1;
	if (str1 == NULL || str2 == NULL)
		return -1;
	for (index = 0; index <= len1 - len2; index++) {
		if (!memcmp(str1+index, str2, len2))
			return index;
	}
	return -1;
}

kws_string kws_field_find(kws_request *request, char *field, size_t len)
{
	kws_field_kv *kv;
	kws_string none;

	if (request == NULL || field == NULL || len == 0) {
		none.pstart = NULL;
		none.len = 0;
		return none;
	}
	list_for_each_entry(kv, &(request->fields.list), list)
	{
		if (kws_strcmp(kv->key, field, len) == 0) {
			return kv->value;
		}
	}
	none.pstart = NULL;
	none.len = 0;
	return none;
}

/*
 * Convert string to positive integer
 * Every char in string should be digit
 * Any non-digit will lead to reture NONDIGIT
 * Integer overflow will lead to return INTOVERFLOW
 */
int kws_atoui(kws_string str) {
	char *c, *end;
	int result, digit;

	if (INVALID_STR(str)) {
		return NOINTSTR;
	}

	c = str.pstart;
	end = str.pstart + str.len;
	while (*c == ' ')
		c++;

	result = 0;
	while (c < end) {
		if ('0' <= *c && *c <= '9') {
			digit = *c - '0';
			if( result > (INT_MAX - digit) /10 ) {
                return INTOVERFLOW;
            }
			result = result * 10 + digit;
			c++;
		} else {
			return NOINTSTR;
		}
	}
	return result;
}

static int kws_http_ver11_field_handle(kws_request *request)
{
	kws_string connection, content_length;
	size_t i_content_length;
	connection = kws_field_find(request, "Connection", 10);
	content_length = kws_field_find(request, "Content-Length", 14);

	if (!INVALID_STR(content_length)) {
		i_content_length = kws_atoui(content_length);
		if (i_content_length == NOINTSTR) {
			request->status = BADREQUEST;
			request->status_code = 400;
			request->status_msg = "Content-Length invalid";
			return -1;
		}

		if (i_content_length == INTOVERFLOW || i_content_length > request->size) {
			request->status = BADREQUEST;
			request->status_code = 400;
			request->status_msg = "Content-Length too long";
			return -1;
		}

		request->content_length = i_content_length;

	}

	if (INVALID_STR(connection)) {
		/* HTTP/1.1 default enable persistent connection
		 *if no explict define
		 */
		request->connection = KEEPALIVE;
		return 0;
	} else {
		if (kws_strcmp(connection, "Close", 5) == 0) {
			request->connection = CLOSE;
		}

		if (kws_strcmp(connection, "Keep-Alive", 10) == 0) {
			request->connection = KEEPALIVE;
		}
		return 0;
	}
}

/* No implemention */
static int kws_http_ver10_field_handle(kws_request *request)
{
	request->status = BADREQUEST;
	request->status_code = 505;
	request->status_msg = MSG505;
	return -1;
}

/* No implementation */
static int kws_http_ver09_field_handle(kws_request *request)
{
	request->status = BADREQUEST;
	request->status_code = 505;
	request->status_msg = MSG505;
	return -1;
}

/*
 * Parse HTTP header from reading request text
 * @request - parsed request
 * @read_line - the length of latest reading data
 * @Return value -
 *     > 0 : HTTP header parse successfully
 *     = 0 : HTTP header is incomplete, wait next parse
 *     < 0 : HTTP header is invalid, this request is bad
 */
int kws_http_parse(struct kws_request *request, size_t read_len)
{
	int version;
	int pos;
	if (request == NULL) {
		return -1;
	}

	/* -4 covers the case in which previous end of mem drops in range of \r\n\r\n*/
	pos = kws_str_search(request->mem + request->len - 4, read_len + 4, "\r\n\r\n", 4);
	if (pos < 0) {
		return 0;
	}

	request->bound = request->len - 4 + pos + 2; /* \r\n[bound]\n*/
	if (kws_header_parse(request) < 0) {
		return -1;
	}

	version = request->http_version;
	if (version == HTTPVER11) {
		if (kws_http_ver11_field_handle(request) < 0)
		{
			return -1;
		}
	} else if (version == HTTPVER10) {
		if (kws_http_ver10_field_handle(request) < 0) {
			return -1;
		}
	} else if (version == HTTPVER09) {
		if (kws_http_ver09_field_handle(request) < 0) {
			return -1;
		}
	} else {
		request->status = BADREQUEST;
		request->status_code = 505;
		request->status_msg = MSG505;
		return -1;
	}
	return 1;
}
