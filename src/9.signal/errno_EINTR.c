#define   MAX_EINTR_ITERATION	 10
for (j=0; j<MAX_EINTR_ITERATION; j++) {
	if ((ret_poll = poll(pollfds, cnt_fd_socket, 50000)) == -1) {
		/* error */
		switch(errno) {
			case EINTR:
			case EAGAIN:
				continue;
			default:
				/* 그 외의 에러 처리 */
				break;
		}
	} else { 
		break; /* success */
	}
} /* loop: for(j) */
for (i=0; i<ret_poll; i++) {
	for (j=0; j<MAX_EINTR_ITERATION; j++) {
		if ((ret_recv = recv(pollfds[i].fd, buf, SZ_BUF_RECV, 0)) == -1) {
			/* error */
			switch(errno) {
				case EINTR:
				case EAGAIN:
					continue;
				default:
					/* 그 외의 에러 처리 */
					break;
			}
		} else {
			break; /* success */
		}
	} /* loop: for(j) */
	... 생략 ...;
} /* loop: for(i) */
