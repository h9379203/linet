/* vim: set ts=4 sw=4: */
/* Filename    : segv_sigaltstack.c
 * Description : alternative stack for signal handler
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : set SA_ONSTACK for sigaction
 */
#define _XOPEN_SOURCE	700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#define SZ_BUFFER 1024*1024		/* 1MiB */

int exhaust_stack(int count);
void inst_sighandler();
void sa_handler_segv(int signum);
int	flag_altstack;	/* 0:off, nonzero:on(execute sigaltstack) */

#define     SZ_SIGHANDLER_STACK		16384
stack_t	    g_ss;		/* signal stack structure */

int main(int argc, char *argv[])
{
	if (argc == 2 && argv[1][0] == '1') {
		printf("[enabled] alternate signal stack.\n");
		flag_altstack = 1;
	} else {
		printf("[disabled] alternate signal stack.\n");
	}
	printf("SIGSTKSZ(%d) MINSIGSTKSZ(%d)\n", SIGSTKSZ, MINSIGSTKSZ);
	inst_sighandler();
	exhaust_stack(100);
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

void sa_handler_segv(int signum)
{
	time_t	t_now = time(0);
	struct tm	*tm_now = localtime(&t_now);
	snprintf(g_ss.ss_sp, g_ss.ss_size, 
			"SEGV: SigNo(%d) Time(%02d:%02d:%02d)",
			signum,
			tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
	printf("%s\n", (char *)g_ss.ss_sp);
	fflush(stdout);
	abort();
}

void inst_sighandler()
{
	struct sigaction sa_segv;
	memset(&sa_segv, 0, sizeof(struct sigaction));
	if (( g_ss.ss_sp = malloc(SZ_SIGHANDLER_STACK)) == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	g_ss.ss_size = SZ_SIGHANDLER_STACK;
	g_ss.ss_flags = 0;

	if (flag_altstack) {
		if (sigaltstack(&g_ss, NULL) == -1) {
			perror("sigaltstack");
			exit(EXIT_FAILURE);
		}
		sa_segv.sa_flags = SA_ONSTACK;
	} else {
		sa_segv.sa_flags = 0;
	}
	sa_segv.sa_handler = sa_handler_segv;
	sigaction(SIGSEGV, &sa_segv, 0);
}
