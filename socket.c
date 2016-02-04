#include "proto.h"

struct socket *kws_sock_alloc(void)
{
	struct socket *sock;
	int error;

	INFO("Enter kws_sock_alloc");
	error = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
	if (error < 0)
		return NULL;

	INFO("Leave kws_sock_alloc");
	return sock;
}

void kws_sock_release(struct socket *sock)
{
	INFO("Enter kws_sock_release");
	sock_release(sock);
	INFO("Leave kws_sock_release");
}

struct socket *kws_sock_listen(int port)
{
	struct socket *sock;
	struct sockaddr_in sin;
	int error;

	INFO("Enter kws_sock_listen");

	sock = kws_sock_alloc();
	if (sock == NULL) {
		ERR("Allocate socket failed");
		return NULL;
	}

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons((unsigned short)port);

	INFO("Start bind");
	error = sock->ops->bind(sock,(struct sockaddr*)&sin,sizeof(sin));
	if (error < 0) {
		ERR("Bind socket failed");
		kws_sock_release(sock);
		return NULL;
	}

	INFO("Start listen");
	error=sock->ops->listen(sock,BACKLOG);
	if (error < 0)
	{
		ERR("Listen socket failed");
		kws_sock_release(sock);
		return NULL;
	}

	INFO("Leave kws_sock_listen");
	return sock;
}

struct socket *kws_accept(struct socket *sock)
{
	struct socket *new_sock;
	int error;

	INFO("Enter kws_accept");

	if (sock == NULL)
		return NULL;

	new_sock = kws_sock_alloc();
	if (new_sock == NULL)
		return NULL;

	new_sock->ops = sock->ops;

	INFO("Start accept");
	error = sock->ops->accept(sock, new_sock, O_RDWR);
	INFO("Finish accept");

	if (error < 0) {
		kws_sock_release(new_sock);
		return NULL;
	}

	INFO("Leave kws_accept");
	return new_sock;
}

int kws_sock_read(struct socket *sock, char *buff,
				size_t len)
{
	int rlen;

	struct kvec vec = {
		.iov_len = len,
		.iov_base = buff
	};

	struct msghdr msg = {
		.msg_flags = MSG_DONTWAIT
	};

	INFO("Enter kws_sock_read");

	rlen = kernel_recvmsg(sock, &msg, &vec, 1, len, MSG_DONTWAIT);
	INFO("Leave kws_sock_read");

	return rlen;
}

int kws_sock_write(struct socket *sock, char *buff,
				size_t len)
{
	return 0;
}
