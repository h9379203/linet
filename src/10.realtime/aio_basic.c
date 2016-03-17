/* vim: set ts=4 sw=4: */
/* Filename    : aio_basic.c
 * Description : 
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>		/* EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <aio.h>
#include <errno.h>

#define FILENAME	"fd_test.log"
#define TEST_MSG	"[Test message:1234567890]\n"

int main()
{
	int		fd, ret;
	struct aiocb aio_wb;

	if ((fd = open(FILENAME, O_CREAT|O_RDWR, 0644)) == -1) {
		perror("open"); 
		exit(EXIT_FAILURE);
	}
	write(fd, TEST_MSG, sizeof(TEST_MSG)); 	/* data written by synchronous I/O API */
	memset(&aio_wb, 0, sizeof(struct aiocb));
	aio_wb.aio_fildes = fd;			/* operated file descript 	*/
	aio_wb.aio_buf = TEST_MSG;		/* buffer to write */
	aio_wb.aio_nbytes = sizeof(TEST_MSG);	/* number of bytes */
	aio_wb.aio_offset = 5;		/* start offset to write */
	aio_write(&aio_wb);			/* data written by asynchronous I/O API */
	while ((ret = aio_error(&aio_wb)) != 0) {
		if (ret == EINPROGRESS) {
			printf("aio_write has not been completed. sleep(1)\n");
			sleep(1);
		} else {
			printf("Error: aio_error = %d\n", ret);
			break;
		}
	}
	if ((ret = aio_return(&aio_wb)) == -1) {
		fprintf(stderr, "Err(aio_write) : %s\n", strerror(errno));
	}
	printf("> write complete: %d B\n", ret);
	fsync(fd);
	close(fd);
	return EXIT_SUCCESS;
}
