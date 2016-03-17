/* vim: set ts=4 sw=4: */
/* Filename    : sig_pending.c
 * Description : simple signal model with suspend/mask/pending
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : sigaction & sigprocmask, sigsuspend, sigpending
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
void sa_handler_usr(int signum);

int main()
{
	int 	i;
	struct sigaction sa_usr1, sa_usr2;
	sigset_t		sigset_mask, sigset_oldmask, sigset_pend;

	memset(&sa_usr1, 0, sizeof(struct sigaction));
	sa_usr1.sa_handler = sa_handler_usr;
	sigfillset(&sa_usr1.sa_mask); /* blocking signal. */
	sa_usr1.sa_flags = 0;
	sigaction(SIGUSR1, &sa_usr1, NULL);


	sa_usr2.sa_handler = SIG_IGN; /* 수동으로 처리하기 위해 USR2는 ignore로. */
	sigaction(SIGUSR2, &sa_usr2, NULL);

	sigfillset(&sigset_mask);	/* 모든 시그널 매스크를 채움 */
	sigdelset(&sigset_mask, SIGINT); /* 시그널 매스크에서 SIGINT 삭제 */
	printf("PID(%d)\n", getpid());
	for(;;) {
		printf("Install signal block mask (allow only SIGINT)\n");
		sigprocmask(SIG_SETMASK, &sigset_mask, &sigset_oldmask); /* 시그널 블록 매스크 설치 */
		sleep(10);
		sigpending(&sigset_pend); /* check blocked signal */
		for (i=1; i<SIGRTMIN; i++) {
			if (sigismember(&sigset_pend, i)) {
				printf("\tPending signal = %d\n", i);
				switch(i) {
					case SIGUSR2:
						sa_handler_usr(SIGUSR2);	/* 지연된 시그널을 수동으로 처리 */
						break;
					default:
						break;
				}
			}
		} /* loop: for(i) */
		printf("Restore the previous signal block mask.\n");
		sigprocmask(SIG_SETMASK, &sigset_oldmask, NULL); /* 시그널 블록 매스크 원상복구 */
	} /* loop: for */
	return 0;
}

void sa_handler_usr(int signum)
{
	int	i;
	for (i=0; i<3; i++) {
		printf("\tSignal(%s):%d sec.\n", 
				signum == SIGUSR1 ? "USR1":"USR2", i);
		sleep(1);
	}
}
