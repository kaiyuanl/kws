#include "proto.h"
#include "http.h"

void kws_send_200_header(struct socket *sock)
{
	kws_sock_write(sock, HTTPRESP, strlen(HTTPRESP));
}
