/* vim: set ts=4 sw=4: */
/* Filename    : io_epoll_trigger.c
 * Description : compare trigger between level and edge.
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : edge trigger only check modification about statement, but level trigger check level of value.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "stdalsp.h"

#define MIN(a,b)	a > b ? b : a
#define LISTEN_BACKLOG	256
#define DFL_OPEN_MAX_FILES	8192		/* Default value for epoll */

#define ADD_EV(a, b)	if (add_ev(a, b) == -1) { pr_err("Fail: add_ev"); exit(EXIT_FAILURE); }
#define DEL_EV(a, b)	if (del_ev(a, b) == -1) { pr_err("Fail: del_ev"); exit(EXIT_FAILURE); }

int	epollfd;		/* epoll fd */

int add_ev(int efd, int fd);	/* Add epoll variable to epollfd */
int del_ev(int efd, int fd); /* delete epoll variabe from epollfd */
int fcntl_setnb(int fd);

int main(int argc, char *argv[])
{
	/* network var			*/
	socklen_t	len_saddr;
	int		fd, fd_listener;
	int		ret_recv;
	char	*port, buf[1024];
	/* I/O multiplexing var */
	struct epoll_event	*ep_events;
	int		max_open_files;
	int		ret_poll;
	/* logical var 			*/
	int		i;

	if (argc > 2) {
		printf("%s [port number]\n", argv[0]);
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
	ai.ai_family = AF_INET;
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
		pr_err("Fail: socket()");
		exit(EXIT_FAILURE);
	}

	fcntl_setnb(fd_listener); /* set nonblock flag */

	if (bind(fd_listener, ai_ret->ai_addr, ai_ret->ai_addrlen) == -1) {
		pr_err("Fail: bind()");
		exit(EXIT_FAILURE);
	}


	if (!strncmp(port, "0", strlen(port))) { /* port 번호를 알아낸다. */
		struct sockaddr_storage saddr_s;
		len_saddr = sizeof(saddr_s);
		getsockname(fd_listener, (struct sockaddr *)&saddr_s, &len_saddr);
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
		pr_out("bind : %s", port);
	}
	listen(fd_listener, LISTEN_BACKLOG);

	max_open_files = MIN((int)sysconf(_SC_OPEN_MAX), (int)DFL_OPEN_MAX_FILES);
	if ((epollfd = epoll_create(max_open_files)) == -1) {
		/* error */
		exit(EXIT_FAILURE);
	}
	if ((ep_events = calloc(1024, sizeof(struct epoll_event))) == NULL) {
		/* error */
		exit(EXIT_FAILURE);
	}
	pr_out("Epoll create with %d max files", max_open_files);

	ADD_EV(epollfd, fd_listener);
	while (1) {
		/* waiting for data (no timeout) */
		pr_out("Epoll waiting ...");
		if ((ret_poll = epoll_wait(epollfd, ep_events, max_open_files, -1)) == -1) {
			/* error */
		}
		pr_out("EPoll return (%d)", ret_poll);

		/* Are there data to read? */
		for (i=0; i<ret_poll; i++) {
			if (ep_events[i].events & EPOLLIN) {
				/* IS New connection ? */
				if (ep_events[i].data.fd == fd_listener) {
					struct sockaddr_storage saddr_c;
					while(1) {
						len_saddr = sizeof(saddr_c);
						fd = accept(fd_listener, (struct sockaddr *)&saddr_c, &len_saddr);
						if (fd == -1) {
							if (errno == EAGAIN) { /* no more new connection */
								break;
							}
							pr_err("Error get connection from listen socket");
							/* need error processing */
							break;
						}
						ADD_EV(epollfd, fd);
						pr_out("accept : add socket (%d)", fd);
					}
#ifndef ENABLE_EPOLLET
					continue;
#endif
				}

				if ((ret_recv = recv(ep_events[i].data.fd, buf, 2, 0)) == -1) {
					pr_err("fd(%d) recv() error (%s)", ep_events[i].data.fd, strerror(errno));
				} else {
					if (ret_recv == 0) {
						/* closed */
						pr_out("fd(%d) : Session closed", ep_events[i].data.fd);
						DEL_EV(epollfd, ep_events[i].data.fd);
					} else {
						/* normal */
						pr_out("recv(fd=%d,n=%d) = %s", 
								ep_events[i].data.fd, ret_recv, buf);
					}
				}
			} else if (ep_events[i].events & EPOLLERR) {
				/* error */
			} else {
				pr_out("fd(%d) epoll event(%d) err(%s)", 
						ep_events[i].data.fd, ep_events[i].events, strerror(errno));
			}
		}
	}

	return 0;
}

/* Name : add_ev
 * Desc : add socket descriptor to epoll
 * Argv : int efd, int fd
 * Ret  : zero(if success), negative(if fail)
 */
int add_ev(int efd, int fd)
{
	struct epoll_event ev;

#ifdef ENABLE_EPOLLET
	ev.events = EPOLLIN | EPOLLET;	/* check inbound data with edge trigger */
#else
	ev.events = EPOLLIN;	/* check inbound data with level trigger */
#endif
	ev.data.fd = fd;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		pr_out("fd(%d) EPOLL_CTL_ADD  Error(%d:%s)", fd, errno, strerror(errno));
		return -1;
	}

	return 0;
}

/* Name : del_ev
 * Desc : delete socket descriptor from epoll
 * Argv : int efd, int fd
 * Ret  : zero(if success), negative(if fail)
 */
int del_ev(int efd, int fd)
{
	if (epoll_ctl(efd, EPOLL_CTL_DEL, fd, NULL) == -1) {
		pr_out("fd(%d) EPOLL_CTL_DEL Error(%d:%s)", fd, errno, strerror(errno));
		return -1;
	}
	close(fd);
	return 0;
}

/* Name : fcntl_setnb
 * Desc : set non-blocking mode
 * Argv : int fd
 * Ret  : zero(if success), negative(if fail)
 */
int fcntl_setnb(int fd)
{
	/* only influence about O_ASYNC, O_APPEND, O_NONBLOCK on Linux-specific */
	if (fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL)) == -1) {
		return errno;
	}
	return 0;
}

