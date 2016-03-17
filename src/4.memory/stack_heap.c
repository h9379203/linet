/* vim: set ts=4 sw=4: */
/* Filename    : stack_heap.c
 * Description : exhaust stack or heap
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#define SZ_BUFFER 1024*1024		/* 1MiB */

int exhaust_stack(int count);
int exhaust_heap(int count);

int main(int argc, char *argv[])
{
	int		sz_mem;
	sz_mem = atoi(argv[2]);
	switch (argv[1][0]) {
		case 's':
			printf("Stack: %dMB, call func(%p)\n", sz_mem, exhaust_stack);
			exhaust_stack(sz_mem);
			break;
		case 'h':
			printf("Heap : to %d MB, call func(%p)\n", sz_mem, exhaust_heap);
			exhaust_heap(sz_mem);
			break;
		default:
			printf("*** UNKNOWN OPTIONS ***\n");
			exit(EXIT_FAILURE);
	}

	return 0;
}

int exhaust_stack(int count)
{
	char buffer[SZ_BUFFER]; /* 1 MiB stack */

	if (count <= 0) {
		printf(">> stopping recursive func.\n");
		return 0;
	}
	printf("[%d] Current stack addr(%p)\n", count, buffer);
	exhaust_stack(count-1);
	return 0;
}

int exhaust_heap(int count)
{
	char *p_buf;

	if (count <= 0) {
		printf(">> stopping recursive func.\n");
		return 0;
	}
	p_buf = (char *)malloc(SZ_BUFFER);
	if (p_buf == NULL) {
		printf(">> ERR: malloc(), count(%d), err(%s)\n", count, strerror(errno));
		return -1;
	}
	printf("[%d] Current heap addr(%p)\n", count, p_buf);
	exhaust_heap(count-1);
	free(p_buf);	/* release heap */
	return 0;
}
