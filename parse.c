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

struct kws_string kws_getline(char *start, size_t *pos, size_t len)
{
	char *curr;
	int index;
	struct kws_string line;

	curr = string + *pos;
	index = kws_strstr(curr, len-*pos, "\r\n", 2);
	if (index >= 0) {
		line.pstart = curr;
		line.len = index;
		*pos += (index + 2);
	} else {
		line.pstart = curr;
		line.len = -1;
		*pos = len;
	}
	return line;
}

struct kws_field {
	struct kws_string key;
	struct kws_string value;
};

struct kws_string kws_trim(struct kws_string string) {
	return string;
}

struct kws_field kws_get_field(struct kws_string line)
{
	int index;
	struct kws_string key, value;
	struct kws_field field;
	index = kws_strch(line, ':');

	key.pstart = line.pstart;
	key.len = index;

	value.pstart = line.pstart + index + 1;
	value.len = line.len - key.len - 1;

	field.key = key;
	field.value = value;

	return field;
}

void kws_http_parse(struct kws_request *request)
{
	char *mem;
	int pos;
	kws_string line;
	kws_field field;
	INFO("Enter kws_http_parse");
	while ((line = kws_getline(request->mem, &pos, request->len)).len > 0) {
		field = kws_get_field(line);
		kws_hash_add(request->fields, field.key, field.value);
	}
	INFO("Leave kws_http_parse");
}
