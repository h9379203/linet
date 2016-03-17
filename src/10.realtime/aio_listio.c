/* vim: set ts=4 sw=4: */
/* Filename    : aio_listio.c
 * Description : 
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : Copy file with suffix ".copy" & use asynchronous I/O
                 Create thread when complete listed AIO with LIO_NOWAIT mode.
 * Update      : 2014-08-16 fix bug.
 */
#define _XOPEN_SOURCE	700
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>		/* EXIT_SUCCESS, EXIT_FAILURE */
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <aio.h>
#include <errno.h>
#define NUM_AIOCB_ARRAY		5

#ifdef ASYNCHRONIZED_LIO
#include <pthread.h>
void start_sigev_aio(sigval_t	arg);
pthread_attr_t	pt_attr;
pthread_barrier_t   pt_barrier; /* 2014-08-16 */
#endif

int main(int argc, char *argv[])
{
	int		fd_rd, fd_wr;
	int		i =0, tot_len = 0, ret;
	char	ofname[0xff], rbuf[NUM_AIOCB_ARRAY][200000];
	struct aiocb  aio_blk[NUM_AIOCB_ARRAY];
	struct aiocb  *aiolist[] = { 
		&aio_blk[0], &aio_blk[1],
		&aio_blk[2], &aio_blk[3], &aio_blk[4] };
#ifdef ASYNCHRONIZED_LIO
	struct sigevent sigev = { .sigev_notify = SIGEV_THREAD };
#endif
	if (argc != 2) {
		printf("Usage : %s <source file>\n", argv[0]); 
		return EXIT_SUCCESS;
	}
	sprintf(ofname, "%s.copy", argv[1]);
	printf("file copy : %s =>>> %s\n", argv[1], ofname);
	if ((fd_rd = open(argv[1], O_RDONLY, 0)) == -1) {
		perror("Fail: open()"); 
		exit(EXIT_FAILURE);
	}
	if ((fd_wr = open(ofname, O_CREAT|O_WRONLY|O_TRUNC, 0644)) == -1) {
		perror("Fail: open()"); 
		exit(EXIT_FAILURE);
	}
	memset(aio_blk, 0, sizeof(struct aiocb) * NUM_AIOCB_ARRAY);
	while(1) {
		ret = read(fd_rd, rbuf[i], sizeof(rbuf[i]));
		if (ret == -1) { /* error */ exit(1); }
		aio_blk[i].aio_fildes = fd_wr; 	/* file descriptor */
		aio_blk[i].aio_buf = rbuf[i]; 	/* buffer to write */
		aio_blk[i].aio_nbytes = ret;	/* number of bytes */
		aio_blk[i].aio_offset = tot_len;/* start offset    */
		aio_blk[i].aio_lio_opcode = LIO_WRITE; /* operation mode */

		/* update conditions */
		tot_len += ret;
		i++;

		if (i==NUM_AIOCB_ARRAY || ret == 0) { /* the time to write */
#ifdef ASYNCHRONIZED_LIO
            pthread_barrier_init(&pt_barrier, NULL, 2);

			sigev.sigev_notify = SIGEV_THREAD; 
			sigev.sigev_value.sival_int = tot_len; /* thread argument */

			pthread_attr_init(&pt_attr); /* thread attribute */
			pthread_attr_setdetachstate(&pt_attr, PTHREAD_CREATE_DETACHED);
			sigev.sigev_notify_attributes = &pt_attr;
			sigev.sigev_notify_function = start_sigev_aio; /* thread function */
			lio_listio(LIO_NOWAIT, aiolist, ret == 0 ? i-1:i, &sigev); /* async. lio */
            pthread_barrier_wait(&pt_barrier);    /* barrier (2014-08-16) */
            pthread_barrier_destroy(&pt_barrier); /* destory barrier before re-init */
#else
			lio_listio(LIO_WAIT, aiolist, ret == 0 ? i-1:i, NULL); /* sync. lio */
#endif 

			memset(aio_blk, 0, sizeof(struct aiocb) * NUM_AIOCB_ARRAY);
            i=0; /* reset index */
		}

		if (ret == 0) break; /* EOF then break */
	} /* loop: while */
	printf("> write complete\n");
	close(fd_rd); 
	fsync(fd_wr);
	close(fd_wr);
	return (EXIT_SUCCESS);
}

#ifdef ASYNCHRONIZED_LIO
void start_sigev_aio(sigval_t arg)
{
	printf("[SIGEV] thread(%ld) by lio_listio (len:%d).\n", (long)pthread_self(), arg.sival_int);
    pthread_barrier_wait(&pt_barrier);    /* barrier (2014-08-16) */
}
#endif
