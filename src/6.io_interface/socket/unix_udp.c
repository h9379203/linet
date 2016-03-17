/* vim: set ts=4 sw=4: */
/* Filename    : unix_udp.c
 * Description : UNIX domain socket with UDP
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : SUSv4(IEEE Std. 1003.1-2008) applied.
 */
#define _XOPEN_SOURCE	700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <arpa/inet.h>
#include "stdalsp.h"

#define PATH_UNIX_SOCKET	"/tmp/my_alsp_socket"

int main(int argc, char *argv[])
{
	int		ufd, sz_rlen;
	socklen_t	len_saddr;
	struct sockaddr_un	saddr_u;
	int		flag_recver;	/* 0:receiver, otherwise:sender */
	char	a_buf[256];

	ssize_t n_read = 0; size_t  n_input = 256;
	char    *p_input = (char *) malloc(n_input);

	memset(&saddr_u, 0x00, sizeof(saddr_u));
	if (argc != 2) {
		printf("%s <receiver | sender>\n", argv[0]); exit(EXIT_FAILURE);
	}
	if (argv[1][0] == 'r') 
		flag_recver = 1;
	else 
		flag_recver = 0;

	ufd = socket(AF_UNIX, SOCK_DGRAM, IPPROTO_IP);
	if (ufd == -1) {
		pr_err("[UNIX_Socket] : Fail: socket()"); exit(EXIT_FAILURE);
	}

	/* setting socket address structure */
	saddr_u.sun_family = AF_UNIX;			/* AF_UNIX, PF_UNIX are same */
	snprintf(saddr_u.sun_path, sizeof(saddr_u.sun_path), PATH_UNIX_SOCKET);

	printf("[UNIX Domain] socket path: %s\n", PATH_UNIX_SOCKET);
	if (flag_recver) { /* receiver */
		/* remove exist socket */
		if (remove(PATH_UNIX_SOCKET)) {
			if (errno != ENOENT) { /* exit when occur error except "No such file.." */
				pr_err("[UNIX Socket] : Fail: remove()"); 
                exit(EXIT_FAILURE);
			}
		}

		len_saddr = sizeof(saddr_u);
		if (bind(ufd, (struct sockaddr *)&saddr_u, sizeof(saddr_u)) == -1) {
			pr_err("[UNIX Socket] : Fail: bind()"); 
            exit(EXIT_FAILURE);
		}
		while (1) {
			sz_rlen = recvfrom(ufd, a_buf, sizeof(a_buf), 0, 
					NULL, NULL);
			if (sz_rlen == -1) {
				pr_err("[UDP Socket] : Fail: recvfrom()");
			}
			pr_out("[recv] (%d byte) (%.*s)", sz_rlen, sz_rlen, a_buf);
		}
	} else { /* sender */
		while (1) {
			if ((n_read = getline(&p_input, &n_input, stdin)) == -1) { /* error */
				return -1;
			}

			len_saddr = sizeof(saddr_u);
			sz_rlen = sendto(ufd, p_input, n_read-1, 0,
					(struct sockaddr *)&saddr_u, len_saddr);
			if (sz_rlen == -1) {
				pr_err("[UDP Socket] : Fail: sendto()");
			}
			pr_out("[send] (%d byte) (%.*s)", sz_rlen, sz_rlen, p_input);
		}
	}
	return 0;
}
