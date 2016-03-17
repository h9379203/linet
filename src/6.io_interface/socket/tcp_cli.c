/* vim: set ts=4 sw=4: */
/* Filename    : in_stream_cli.c
 * Description : will send NORMAL band or OUT of band data over TCP
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 
 * Rel.        : 2nd Ed.
 */
#define _XOPEN_SOURCE	700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include "stdalsp.h"
#ifdef ENABLE_O_NONBLOCK
#include <fcntl.h>
#include <sys/time.h>
#include <sys/select.h>
#endif

#define str_ordinary	"1.abcde"
#define str_OOB			"2.fghij"

int main(int argc, char *argv[])
{
	int		fd, rc_gai;
	struct addrinfo	ai, *ai_ret;

	if (argc != 3) {
		printf("%s <hostname> <port>\n", argv[0]); exit(EXIT_FAILURE);
	}

	memset(&ai, 0, sizeof(ai));
	ai.ai_family = AF_UNSPEC;
	ai.ai_socktype = SOCK_STREAM;
	ai.ai_flags = AI_ADDRCONFIG;
	if ((rc_gai = getaddrinfo(argv[1], argv[2], &ai, &ai_ret)) != 0) {
		pr_err("Fail: getaddrinfo():%s", gai_strerror(rc_gai));
		exit(EXIT_FAILURE);
	}

	if ((fd = socket(ai_ret->ai_family, 
					ai_ret->ai_socktype, 
					ai_ret->ai_protocol)) == -1) {
		pr_err("[Client] : Fail: socket()"); 
        exit(EXIT_FAILURE);
	}
#ifndef ENABLE_O_NONBLOCK
	if (connect(fd, ai_ret->ai_addr, ai_ret->ai_addrlen) == -1) {
		pr_err("[TCP client] : Fail: connect()"); 
        exit(EXIT_FAILURE);
	}
#else
	if (fcntl(fd, F_SETFL, O_NONBLOCK|fcntl(fd, F_GETFL)) == -1) {
		/* error 처리 */
	}

	(void)connect(fd, ai_ret->ai_addr, ai_ret->ai_addrlen); 
	if (errno != EINPROGRESS) {
		/* error 처리 */
	}
	pr_out("[nonblocking] try to make a connection");
	fd_set	fdset_w;
	FD_ZERO(&fdset_w);
	FD_SET(fd, &fdset_w);
	if (select(fd+1, NULL, &fdset_w, NULL, NULL) == -1) {
		/* error 처리 */
	}

	int 	sockopt;
	socklen_t	len_sockopt = sizeof(sockopt);
	if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &sockopt, &len_sockopt) == -1) {
		/* error 처리 */
	}
	if (sockopt) {
		/* connect중에 에러 발생한 경우는 sockopt에 errno에 해당하는 번호가 기록됨*/
		pr_err("SO_ERROR: %s(%d)", strerror(sockopt), sockopt);
	}
	pr_out("[nonblocking] connection established");

#endif

#ifndef ENABLE_MSG_OOB
	pr_out("[TCP client] : 1:ordinary data  0:Exit program");
#else
	pr_out("[TCP client] : 1:ordinary data  2:OOB data  0:Exit program");
#endif
	int		rc_getline, rc_send, flag_send;
	char	*p_sbuf, *p_buf = NULL;
	size_t  len_buf;
	while (1) {
		if ((rc_getline = getline(&p_buf, &len_buf, stdin)) == -1) { /* error */
			return EXIT_FAILURE;
		}

		switch(atoi(p_buf)) {
			case 0:
				exit(EXIT_SUCCESS);
				break;

			case 1:
				p_sbuf = str_ordinary;
				flag_send = 0;
				printf(">> will send ordinary msg: data = [%s]\n", p_sbuf);
				break;

#ifdef ENABLE_MSG_OOB
			case 2:
				p_sbuf = str_OOB;
				flag_send = MSG_OOB;
				printf(">> will send OOB msg: data = [%s]\n", p_sbuf);
				break;
#endif
			default:
				printf(">> Error : (%s)\n", p_buf);
				continue;
		}
		free(p_buf); 
		p_buf = NULL;

		if ((rc_send = send(fd, p_sbuf, strlen(p_sbuf), flag_send)) == -1) {
			pr_err("[TCP client] : Fail: send()");
		}
	}

	return 0;
}
