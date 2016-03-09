#include "proto.h"

int kws_pool_worker(void *param)
{
	struct kws_pool_task *pool_task;
	struct kws_request *request;

	if (param == NULL) {
		return -1;
	}

	pool_task = (struct kws_pool_task *)param;
	if (pool_task->task != current) {
		return -1;
	}

	while (pool_task->status == INIT) {
		schedule();
	}

	while(KwsStatus != EXIT) {
		if (pool_task != NULL && pool_task->status == NEWTASK){
			request = (struct kws_request *)(pool_task->task_param);
			(pool_task->task_handler)(request);
			pool_task->status = TASKDONE;
			continue;
		}

		if (pool_task != NULL && pool_task->status == TASKDONE) {
			schedule();
		}
	}
}

struct kws_pool_task *kws_pool_task_alloc(void)
{
	struct kws_pool_task pool_task;
	struct task_struct task;
	pool_task = (struct kws_pool_task *)kmalloc(sizeof(struct kws_pool_task), GFP_KERNEL);
	if (pool_task == NULL) {
		return NULL;
	}

	pool_task->task_handler = &kws_task_handler;
	pool_task->task_param = NULL;
	INIT_LIST_HEAD(&(pool_task->lock));
	pool_task->status = INIT;

	task = kthread_create(&kws_pool_worker, (void *)pool_task, "Thread in pool");
	if (IS_ERR(task)) {
		kfree(pool_task);
		return NULL;
	}

	pool_task->task = task;
	return pool_task;
}

int kws_task_pool_init(struct kws_pool **pool, size_t size);
{
	struct kws_pool_task *pool_task;
	int i;
	if (pool == NULL) {
		return -1;
	}
	(*pool) = (struct kws_pool*)kmalloc(sizeof(struct kws_pool), GFP_KERNEL);
	if ((*pool) == NULL) {
		return -1;
	}
	spin_lock_init(&((*pool)->lock));
	INIT_LIST_HEAD(&((*pool)->tasks));
	(*pool)->size = 0;

	for (i = 0; i < size; i++) {
		pool_task = kws_pool_task_alloc();
		if (pool_task == NULL) {
			continue;
		}
		list_add(&(pool_task->list), &((*pool)->tasks.list));
		i++;
	}
	return 0;
}

struct kws_pool_task *kws_task_pool_pickup(struct kws_pool *pool)
{
	spin_lock(&(pool->lock));
	if (pool->size == 0) {

	} else if (pool->size > 0) {

	} else {
		ERR("Should never happen");
	}
	spin_unlock(&(pool->lock));
}

int kws_task_pool_handle(void *task_param)
{
	struct kws_pool_task *pool_task;
	pool_task = kws_task_pool_pickup(ThreadPool);
	if (pool_task == NULL) {
		return -1;
	}

	pool_task->task_param = task_param;
	pool_task->status = NEWTASK;
	wake_up_process(pool_task->task);
	return 0;
}
