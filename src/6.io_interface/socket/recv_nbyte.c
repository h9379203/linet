int  recv_nbyte(int sd, char *rbuf, int len) {
	int ret, len_recv = 0; /* len_recv : 현재까지 읽어들인 누적 바이트 수 */
	while (len_recv < len) {
		if ((ret = recv(sd, &rbuf[len_recv], len - len_recv, 0)) == 0) {
			/* recv가 0을 리턴하면 연결 종료를 의미 */
			return 0;
		}
		if (ret == -1) {
			if (errno == EAGAIN || errno == EINTR) {
				continue;   /* EAGAIN or EINTR의 경우는 재시도 */
			} else {
				perror("Fail: recv");
			}
		}
		len_recv += ret;    /* len_recv에 수신된 데이터 바이트 수를 누적시킴 */
	}
	return len_recv;
}
