#include "proto.h"
#include "http.h"

static void kws_str_print(kws_string str)
{
	int i;
	for (i = 0; i < str.len; i++) {
		printk("%c", str.pstart[i]);
	}
}

int kws_is_static_file(struct kws_string fname)
{
	return 1;
}

void kws_static_handle(struct kws_request *request)
{
	struct file *filp;

	/* Kws return index.html as response regardless url of request.
	 * This is a demo
	 */
	filp = filp_open("/usr/wwwroot/index.html", O_RDONLY, 0);
	if (filp == NULL) {
		ERR("Cannot open file");
		return;
	}

	kws_send_200_header(request->sock);
	kws_sock_write(request->sock, "\r\n\r\n", 4);
	kws_sock_write(request->sock, INDEXHTML, strlen(INDEXHTML));
}

void kws_dynamic_handle(struct kws_request *request)
{

}

void kws_default_handle(void *data)
{
	struct kws_request *request;
	struct kws_field_kv *kv;
	if (data == NULL) {
		return;
	}
	request = (struct kws_request *)data;
	INFO("-----THIS IS IN FRAMEWORK-----");
	printk("%d", request->method); printk(" ");
	kws_str_print(request->url); printk(" ");
	printk("HTTP/%d\n", request->http_version);
	list_for_each_entry(kv, &(request->fields.list), list)
	{
		kws_str_print(kv->key);
		printk(" : ");
		kws_str_print(kv->value);
		printk("\n");
	}
	INFO("------------------------------");

	if (kws_is_static_file(request->url)) {
		kws_static_handle(request);
	} else {
		kws_dynamic_handle(request);
	}
}


