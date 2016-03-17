/* vim: set ts=4 sw=4: */
/* Filename    : inet_tcp_serv1_OOB.c
 * Description : OOB model
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : prefork model of TCP echo server
 * Rel.        : 2nd Ed.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>		/* for inet_ntoa */
#include <errno.h>
#include <sys/wait.h>
#include "stdalsp.h"

#define LISTEN_BACKLOG	256
#define MAX_POOL	3

int fd_listener;
void start_child(int fd, int idx);

int main(int argc, char *argv[])
{
	int		i; 
	short port; 
	socklen_t len_saddr;
	struct sockaddr_in	saddr_s = {};
	pid_t	pid;

	if (argc > 2) {
		printf("%s [port number]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	if (argc == 2) {
		port = (short) atoi((char *)argv[1]);
	} else {
		port = 0;	/* random port */
	}

	if ((fd_listener = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) == -1) {
		pr_err("[TCP server] : Fail: socket()"); 
		exit(EXIT_FAILURE);
	}

	saddr_s.sin_family = AF_INET;			/* IPv4 */
	saddr_s.sin_addr.s_addr = INADDR_ANY;	/* 0.0.0.0 */
	saddr_s.sin_port = htons(port);

	if (bind(fd_listener, (struct sockaddr *)&saddr_s, sizeof(saddr_s)) == -1) {
		pr_err("[TCP server] Fail: bind()"); 
		exit(EXIT_FAILURE);
	}

	if (port == 0) {
		len_saddr = sizeof(saddr_s);
		getsockname(fd_listener, (struct sockaddr *)&saddr_s, &len_saddr);
	}
	pr_out("[TCP server] Port : #%d", ntohs(saddr_s.sin_port));
	listen(fd_listener, LISTEN_BACKLOG);
	for(i=0; i<MAX_POOL; i++) {
		/* fork immediately */
		switch(pid = fork()) {
			case 0:		/* Child process */
				start_child(fd_listener, i);
				exit(EXIT_SUCCESS);
				break;
			case -1:	/* Error */
				pr_err("[TCP server] : Fail: fork()");
				break;
			default:	/* parent */
				pr_out("[TCP server] Making child process No.%d", i);
				break;
		}
	}
	for(;;) 
		pause();
	return 0;
}

/* Name : start_child
 * Desc : echo about socket stream.
 * Argv : int fd (socket descriptor)
 * Ret  : None
 */
void start_child(int sfd, int idx)
{
	int		cfd;
	int		ret_recv, cum_recv;
	int		ret_sockatmark;
	socklen_t len_saddr;
	char	buf[1024];
	struct sockaddr_storage	saddr_c;

	for(;;) {
		ret_recv = cum_recv = 0;
		len_saddr = sizeof(saddr_c);
		if ((cfd = accept(sfd, (struct sockaddr *)&saddr_c, &len_saddr)) == -1) {
			pr_err("[Child] Fail: accept()");
			continue;
		}

		for(;;) {
			// ret = ioctl(cfd, SIOCATMARK, &ret_sockatmark); // old-fashioned
			ret_sockatmark = sockatmark(cfd);
			if (ret_sockatmark == 0) { /* normal band */
WAIT_RECV:
				pr_out("[normal band] cum_recv (%d byte)", cum_recv);
				if ((ret_recv = recv(cfd, buf+cum_recv, sizeof(buf)-cum_recv, 0))
						== -1) {
					pr_err("[Child:%d] Fail: recv(): %s", idx, strerror(errno));
				}
				if (ret_recv == 0) { /* closed */
					pr_err("[Child:%d] Session closed", idx);
					close(cfd);
					break;
				}
				cum_recv += ret_recv;
			} else if (ret_sockatmark == 1) { /* OOB */
				pr_out("ret_sockatmakr = %d", ret_sockatmark);
				if ((ret_recv = recv(cfd, buf+cum_recv, 2, MSG_OOB)) == -1) {
					if (errno == EINVAL) {    /* normal-band를 기다려야 한다. */
						pr_err("goto recv(norm-inband)");
						goto WAIT_RECV;
					}
					pr_err("ERR: OOB:ret_recv (%s)\n", strerror(errno));
					continue;
				}
				cum_recv++;
				pr_out("Recv(with OOB,%d byte) [%.*s]\n", cum_recv, cum_recv, buf);
				cum_recv = 0; /* reset cumulative counter */
			} else {	/* == -1 */
				pr_err("Fail: sockatmark : %s", strerror(errno));
			}
		} /* packet recv loop */
	} /* main for loop */
}
