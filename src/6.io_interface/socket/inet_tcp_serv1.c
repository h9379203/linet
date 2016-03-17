/* vim: set ts=4 sw=4: */
/* Filename    : inet_tcp_serv1.c
 * Description : prefork child and wait for tcp client connection
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : prefork model of TCP echo server.
 *               some obsolete function included.
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
	int		cfd, ret_len; 
	socklen_t len_saddr;
	char	buf[40];	/* small buf */
	struct sockaddr_storage	saddr_c;

	for(;;) {
		len_saddr = sizeof(saddr_c);
		if ((cfd = accept(sfd, (struct sockaddr *)&saddr_c, &len_saddr)) == -1) {
			pr_err("[Child] Fail: accept()");
			continue;
		}
		if (saddr_c.ss_family == AF_INET) {
			pr_out("[Child:%d] accept (ip:port) (%s:%d)", idx, 
				inet_ntoa( ((struct sockaddr_in *)&saddr_c)->sin_addr ),
				ntohs( ((struct sockaddr_in *)&saddr_c)->sin_port ) );
		}

		for(;;) {
			ret_len = recv(cfd, buf, sizeof(buf), 0);
			if (ret_len == -1) {
				if (errno == EINTR) continue;
				pr_err("[Child:%d] Fail: recv(): %s", idx, strerror(errno));
				break;
			}
			if (ret_len == 0) {
				pr_err("[Child:%d] Session closed", idx);
				close(cfd);
				break;
			}

			pr_out("[Child:%d] RECV(%.*s)", idx, ret_len, buf);
			if (send(cfd, buf, ret_len, 0) == -1) {
				pr_err("[Child:%d] Fail: send() to socket(%d)", idx, cfd);
				close(cfd);
			}
			sleep(1); /* it's not special meaning */
		} /* packet recv loop */
	} /* main for loop */
}
