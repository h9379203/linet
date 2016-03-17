/* Filename    : pspawn4.c
 * Description : test POSIX_SPAWN_SETSIGDEF
 * Notes       :
 * 1st Release : 
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <spawn.h>
#include <sys/wait.h>

int main()
{
	int		ret_err = 0;
	pid_t	pid_child;
	char	buf_err[64];
	char	*argv_child[] = {
		"pspawn4_child", NULL
	};
	printf("Parent[%d]: Start\n", getpid());
	struct sigaction    sa_usr1, sa_usr2, sa_term;
	memset(&sa_usr1, 0, sizeof(struct sigaction));
	sa_usr1.sa_handler = SIG_IGN;
	sa_term = sa_usr2 = sa_usr1;
	sigaction(SIGUSR1, &sa_usr1, NULL);	// ignore SIGUSR1
	sigaction(SIGUSR2, &sa_usr2, NULL);	// ignore SIGUSR2
	sigaction(SIGTERM, &sa_term, NULL); // ignore SIGTERM

	posix_spawnattr_t	posix_attr;
	if ((ret_err = posix_spawnattr_init(&posix_attr)) != 0) {
		strerror_r(ret_err, buf_err, sizeof(buf_err));
		fprintf(stderr, "Fail: attr_init: %s\n", buf_err);
		exit(EXIT_FAILURE);
	}
	short posix_flags = POSIX_SPAWN_SETSIGDEF;
	if ((ret_err = posix_spawnattr_setflags(&posix_attr, posix_flags)) != 0) {
		strerror_r(ret_err, buf_err, sizeof(buf_err));
		fprintf(stderr, "Fail: attr_setflags: %s\n", buf_err);
		exit(EXIT_FAILURE);
	}

	sigset_t    sigset_def;
	sigemptyset(&sigset_def);
	sigaddset(&sigset_def, SIGTERM);	// set SIGTERM's bit
	if ((ret_err = posix_spawnattr_setsigdefault(&posix_attr, &sigset_def)) != 0) {
		strerror_r(ret_err, buf_err, sizeof(buf_err));
		fprintf(stderr, "Fail: attr_setsigdefault: %s\n", buf_err);
		exit(EXIT_FAILURE);
	}
	ret_err = posix_spawn( &pid_child,
			argv_child[0],
			NULL,
			&posix_attr,		/* attribute */
			argv_child,
			NULL);
	if ((ret_err = posix_spawnattr_destroy(&posix_attr)) != 0) {
		strerror_r(ret_err, buf_err, sizeof(buf_err));
		fprintf(stderr, "Fail: attr_destroy: %s\n", buf_err);
		exit(EXIT_FAILURE);
	}
	printf("Parent[%d]: Wait for child(%d)\n", getpid(), (int)pid_child);
	(void)wait(NULL);	/* wait for child */
	printf("Parent[%d]: Exit\n", getpid());
	return 0;
}

