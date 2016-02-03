#include "proto.h"

struct kws_queue *kws_queue_alloc(unsigned int size)
{
	struct kws_queue *queue;
	queue = (struct kws_queue *)kmalloc(sizeof(struct kws_queue), GFP_KERNEL);
	if (queue == NULL)
		return NULL;

	spin_lock_init(&(queue->lock));
	init_waitqueue_head(&(queue->wq));

	queue->size = size;

	queue->items = (void **)kmalloc(size * sizeof(void *), GFP_KERNEL);
	if(queue->items == NULL) {
		kfree(queue);
		return NULL;
	}

	memset(queue->items, 0, size * sizeof(void *));
	queue->in = queue->out = 0;
	queue->count = 0;
	return queue;
}

bool kws_queue_empty(struct kws_queue *queue)
{
	return (queue->count == 0);
}

bool kws_queue_full(struct kws_queue *queue)
{
	return (queue->count == queue->size);
}

int kws_queue_in(struct kws_queue *queue, void *item)
{
	if (queue == NULL)
		return -1;
	if (item == NULL)
		return -1;

	spin_lock(&(queue->lock));
	if (queue->count >= queue->size){
		spin_unlock(&(queue->lock));
		return -1;
	}

	queue->items[(queue->in)%(queue->size)] = item;
	(queue->in)++;
	(queue->count)++;
	spin_unlock(&(queue->lock));
	return 0;
}

void *kws_queue_out(struct kws_queue *queue)
{
	void *item;
	if (queue == NULL)
		return NULL;
	spin_lock(&(queue->lock));
	if (queue->count <= 0) {
		spin_unlock(&(queue->lock));
		return NULL;
	}

	item = queue->items[(queue->out)%(queue->size)];
	queue->items[(queue->out)%(queue->size)] = NULL;
	(queue->out)++;
	(queue->count)--;
	spin_unlock(&(queue->lock));
	return item;
}

void kws_queue_release(struct kws_queue *queue)
{
	kfree(queue->items);
}

struct kws_queue *kws_request_queue_alloc(unsigned int size)
{
	return kws_queue_alloc(size);
}

bool kws_request_queue_empty(struct kws_queue *queue)
{
	return kws_queue_empty(queue);
}

bool kws_request_queue_full(struct kws_queue *queue)
{
	return kws_queue_full(queue);
}

int kws_request_queue_in(struct kws_queue *queue, struct kws_request *request)
{
	return kws_queue_in(queue, (void *)request);
}

struct kws_request *kws_request_queue_out(struct kws_queue *queue)
{
	return (struct kws_request *)kws_request_queue_out(queue);
}

void kws_request_queue_release(struct kws_queue *queue)
{
	int i;
	for (i = 0; i < queue->size; i++) {
		if (queue->items[i] != NULL) {
			kws_request_release((struct kws_request *)(queue->items[i]));
		}
	}
	kws_queue_release(queue);
}
