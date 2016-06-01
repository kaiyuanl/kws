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
	char url[URLMAX] = {0};
	size_t file_len;
	ssize_t ret;
	mm_segment_t fs;

	strncat(url, WWWRoot, strlen(WWWRoot));
	strncat(url, request->url.pstart, request->url.len);
	INFO("FS path : %s", url);

	filp = filp_open(url, O_RDONLY, 0);
	if (filp == NULL || IS_ERR(filp)) {
		INFO("Cannot open file");
		return;
	}
	/* ToDo: Read and send file to socket */
	file_len = filp->f_path.dentry->d_inode->i_size;
	request->response = (struct kws_response *)kmalloc(sizeof(struct kws_response), GFP_KERNEL);
	if (request->response == NULL) {
		INFO("Response allocated failed");
		return;
	}
	request->response->type = STATIC;
	request->response->size = file_len;
	request->response->mem = (char *)kmalloc(file_len, GFP_KERNEL);
	if (request->response == NULL) {
		INFO("Mem allocated failed");
		kfree(request->response);
		return;
	}
	fs = get_fs();
	// Set segment descriptor associated to kernel space
	set_fs(get_ds());
	// Read the file
	ret = vfs_read(filp, request->response->mem, file_len, &filp->f_pos);
	if(ret < 0)
	{
		ERR("Read file failed");
	}
	// Restore segment descriptor
	set_fs(fs);
	kws_sock_write(request->sock, request->response->mem, file_len);
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
	if (request->connection == KEEPALIVE) {
		request->status = READING;
		while (kws_request_queue_in(RequestQueue, request) < 0) {
			wait_event_interruptible(RequestQueue->wq, RequestQueue-> count < RequestQueue->size || KwsStatus == EXIT);
			if (KwsStatus == EXIT) {
				kws_request_release(request);
				return;
			}
		}
	} else {
		kws_request_release(request);
	}
}


