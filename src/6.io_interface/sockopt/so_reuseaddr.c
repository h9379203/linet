/* vim: set ts=4 sw=4: */
/* Filename    : so_rcvtimeo.c
 * Description : apply SO_REUSEADDR socket option
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : prefork model of TCP echo server
 * Todo        : make routine for signal SIGCHLD
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
	int		i, sockopt;
	char	*port;
	socklen_t len_saddr;
	pid_t	pid;

	if (argc > 2) {
		printf("%s [port number]", argv[0]);
		exit(EXIT_FAILURE);
	}
	if (argc == 2) {
		port = strdup(argv[1]);
	} else {
		port = strdup("0");	/* random port */
	}

	struct addrinfo ai, *ai_ret;
	int     rc_gai;
	memset(&ai, 0, sizeof(ai));
	ai.ai_family = AF_INET;	/* AF_UNSPEC, AF_INET, AF_INET6 */
	ai.ai_socktype = SOCK_STREAM;
	ai.ai_flags = AI_ADDRCONFIG | AI_PASSIVE;

	if ((rc_gai = getaddrinfo(NULL, port, &ai, &ai_ret)) != 0) {
		pr_err("Fail: getaddrinfo():%s", gai_strerror(rc_gai));
		exit(EXIT_FAILURE);
	}

	if ((fd_listener = socket(
					ai_ret->ai_family,
					ai_ret->ai_socktype,
					ai_ret->ai_protocol)) == -1) {
		pr_err("[TCP server] : Fail: socket()");
		exit(EXIT_FAILURE);
	}

	sockopt = 1;
	if (setsockopt(fd_listener, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) < 0) {
		pr_err("[TCP server] : Fail: setsockopt()"); exit(EXIT_FAILURE);
	}

	if (bind(fd_listener, ai_ret->ai_addr, ai_ret->ai_addrlen) == -1) {
		pr_err("[TCP server] Fail: bind()");
		exit(EXIT_FAILURE);
	}

	if (!strncmp(port, "0", strlen(port))) { /* port 번호를 알아낸다. */
		struct sockaddr_storage saddr_s;
		len_saddr = sizeof(saddr_s);
		getsockname(fd_listener, (struct sockaddr *)&saddr_s, &len_saddr);
		if (saddr_s.ss_family == AF_INET) {
			pr_out("[TCP server] IPv4 Port : #%d",
					ntohs(((struct sockaddr_in *)&saddr_s)->sin_port));
		} else  if (saddr_s.ss_family == AF_INET6) {
			pr_out("[TCP server] IPv6 Port : #%d",
					ntohs(((struct sockaddr_in6 *)&saddr_s)->sin6_port));
		} else {
			pr_out("[TCP server] (ss_family=%d)", saddr_s.ss_family);
		}
	}
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
				printf("[TCP server] Making child process No.%d\n", i);
				break;
		}
	}
	for(;;) pause();
	return 0;
}

/* Name : start_child
 * Desc : echo about socket stream.
 * Argv : int fd (socket descriptor)
 * Ret  : None
 */
void start_child(int sfd, int idx)
{
	int		cfd, ret_len;
	unsigned int len_saddr;
	char	buf[40];	/* small buf */
	struct sockaddr_storage	saddr_c;

	for(;;) {
		len_saddr = sizeof(saddr_c);
		if ((cfd = accept(sfd, (struct sockaddr *)&saddr_c, &len_saddr)) == -1) {
			pr_err("[Child] Fail: accept()");
			continue;
		}

		for(;;) {
			ret_len = recv(cfd, buf, sizeof(buf), 0);
			if (ret_len == -1) {
				if (errno == EINTR) 	continue;
				if (errno == EAGAIN) {
					printf("[Child:%d] recv() get timeout error: %s\n", idx, strerror(errno));
					continue; /* time out */
				}
				fprintf(stderr, "[Child:%d] Fail: recv(): %s\n", idx, strerror(errno));
				break;
			}
			if (ret_len == 0) {
				fprintf(stderr, "[Child:%d] Session closed\n", idx);
				close(cfd);
				break;;
			}

			printf("[Child:%d] RECV(%.*s)\n", idx, ret_len, buf);
			if (send(cfd, buf, ret_len, 0) < 0) { 
				fprintf(stderr, "[Child:%d] Fail: send() to socket(%d)\n", idx, cfd);
				close(cfd);
			}
			sleep(1); /* it's not special meaning */
		} /* packet recv loop */
	} /* main for loop */
}
