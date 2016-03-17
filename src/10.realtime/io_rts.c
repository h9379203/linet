/* vim: set ts=4 sw=4: */
/* Filename    : io_rts.c
 * Description : RTS example (linux specification)
 * Author      : Sun Young Kim <sunyzero@yahoo.com>
 * Notes       : Advanced Linux System Network Programming 2nd Ed. (2012)
 * 				 link option : -lrt
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <poll.h>
#include "stdalsp.h"

#define SZ_RECV_BUF		16384

#define SIGRT_LISTEN_IO		SIGRTMIN
#define SIGRT_NORM_IO		SIGRTMIN + 1

int	add_rts_socket(int	fd, int	i_sig);
int start_sigrt(int fd_listener);
int make_listener(int family, char *port_no);	/* make listen socket */

int main(int argc, char *argv[])
{
	char	*port;
	sigset_t	sigset_mask;
	int		fd_listener;

	if (argc > 2) {
		printf("%s [port number]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	if (argc == 2) {
		port = strdup(argv[1]);
	} else {
		port = strdup("0");	/* random port */
	}

	if ((fd_listener = make_listener(AF_INET, port)) == -1) {
		exit(EXIT_FAILURE);
	}

	/* install signal block mask */
	sigemptyset(&sigset_mask);
	sigaddset(&sigset_mask, SIGRT_LISTEN_IO);
	sigaddset(&sigset_mask, SIGRT_NORM_IO);
	if (sigprocmask(SIG_BLOCK, &sigset_mask, NULL) == -1) {
		exit(EXIT_FAILURE);
	}

	if (start_sigrt(fd_listener) == -1) {
		exit(EXIT_FAILURE);
	}

	return 0;
}

/* Name : start_sigrt
 * Desc : 
 * Argv : 
 * Ret  : zero(if success), negative(if fail)
 */

int start_sigrt(int fd_listener)
{
	int		fd_client, ret_recv, i_sig;
	char	rbuf[SZ_RECV_BUF];
	sigset_t	sigset_mask;
	siginfo_t	si_rt;

	add_rts_socket(fd_listener, SIGRT_LISTEN_IO);

	sigemptyset(&sigset_mask);
	sigaddset(&sigset_mask, SIGRT_LISTEN_IO);
	sigaddset(&sigset_mask, SIGRT_NORM_IO);

	while (1) {
		i_sig = sigwaitinfo(&sigset_mask, &si_rt);
		if (i_sig == SIGRT_LISTEN_IO) {
			struct sockaddr_storage	 saddr_c;
			socklen_t len_saddr = sizeof(saddr_c);
			fd_client = accept(fd_listener, (struct sockaddr *)&saddr_c, &len_saddr);
			if (fd_client == -1) {
				pr_err("Error get connection from listen socket");
				/* 에러 처리 */
				break;
			}
			pr_out("[SIGRT] Add socket (%d)", fd_client);

			/* new client */
			add_rts_socket(fd_client, SIGRT_NORM_IO);
		} else if (i_sig == SIGRT_NORM_IO) {
			if (si_rt.si_band & POLLIN) {
				if ((ret_recv = recv(si_rt.si_fd, rbuf, sizeof(rbuf), 0)) == -1) {
					/* error */
					pr_err("fd(%d) recv(%s)", si_rt.si_fd, strerror(errno));
				} else {
					if (ret_recv == 0) {
						/* closed */
						pr_out("close fd(%d)", si_rt.si_fd);
						close(si_rt.si_fd);
					} else {
						/* success */
						pr_out("recv(fd=%d,n=%d) = %.*s", 
								si_rt.si_fd, ret_recv, ret_recv, rbuf);
					}
				}
			} else if (si_rt.si_band & POLLERR) {
				pr_err("POLLERR");
			} else if (si_rt.si_band & POLLHUP) {
				pr_err("POLLHUP");
			} else {
				/* Other signal band */
				pr_err("Unknown band(%ld)", si_rt.si_band);
			}
		} else {
			pr_out("Unknown signal : %d", i_sig);
		}
	} /* loop: while */

	return 0;
}

/* support IPv6, IPv4 address */
int make_listener(int family, char *port_no) 
{
	struct addrinfo ai, *ai_ret;
	int		rc_gai, fd;
	memset(&ai, 0, sizeof(struct addrinfo));
	ai.ai_family = family;
	ai.ai_socktype = SOCK_STREAM;
	ai.ai_flags = AI_ADDRCONFIG | AI_PASSIVE;	/* get local listener addr */
	if ((rc_gai = getaddrinfo(NULL, port_no, &ai, &ai_ret)) != 0) {
		pr_err("Fail: getaddrinfo():%s", gai_strerror(rc_gai));
		exit(EXIT_FAILURE);
	}
	if ((fd = socket(
					ai_ret->ai_family,
					ai_ret->ai_socktype,
					ai_ret->ai_protocol)) == -1) {
		pr_err("Fail: socket()");
		return -1;
	}
	if (bind(fd, ai_ret->ai_addr, ai_ret->ai_addrlen) == -1) {
		pr_err("Fail: bind()");
		close(fd);
		return -1;
	}

	if (!strncmp(port_no, "0", strlen(port_no))) { /* port 번호를 알아낸다.(if random) */
		struct sockaddr_storage    saddr_s;
		socklen_t   len_saddr = sizeof(saddr_s);
		getsockname(fd, (struct sockaddr *)&saddr_s, &len_saddr);
		if (saddr_s.ss_family == AF_INET) {
			pr_out("bind : IPv4 Port : #%d",
					ntohs(((struct sockaddr_in *)&saddr_s)->sin_port));
		} else  if (saddr_s.ss_family == AF_INET6) {
			pr_out("bind : IPv6 Port : #%d",
					ntohs(((struct sockaddr_in6 *)&saddr_s)->sin6_port));
		} else {
			pr_out("getsockname : ss_family=%d", saddr_s.ss_family);
		}
	} else {
		pr_out("bind : %s", port_no);
	}
	listen(fd, 256);
	return fd;
}

int	add_rts_socket(int	fd, int	i_sig)
{
	int	flags;
	if ((flags = fcntl(fd, F_GETFL)) == -1) {
		pr_err("FAIL: fcntl() F_GETFL"); 
		return -1;
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK | O_ASYNC) == -1) {	/* async */
		pr_err("FAIL: fcntl() F_SETFL"); 
		return -1;
	}
	if (fcntl(fd, F_SETSIG, i_sig) == -1) {	/* set signal */
		pr_err("FAIL: fcntl() F_SETSIG"); 
		return -1;
	}
	if (fcntl(fd, F_SETOWN, getpid()) == -1) { /* set ownership */
		pr_err("FAIL: fcntl() F_SETOWN"); 
		return -1;
	}
	return 0;
}
