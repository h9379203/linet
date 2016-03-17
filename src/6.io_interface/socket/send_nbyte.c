int  send_nbyte(int sd, const char *buf, int len) {
	int 	ret, len_sent = 0;
	char    *p_buf = (char *)buf;
	while (len > 0) {
		ret = send(sd, &p_buf[len_sent], len, 0);
		if (ret == -1) {
			if (errno & EAGAIN) {
				/* 소켓버퍼 공간 부족, 재시도 혹은 적절한 처리 */
				continue;
			}
			if (errno & ECONNRESET) {
				return -1;
			}
			/* 그외의 에러 처리. e.g.) EPIPE ... */
		} else {
			len -= ret;
			len_sent += ret;
		}
	}
	return len_sent;
}
