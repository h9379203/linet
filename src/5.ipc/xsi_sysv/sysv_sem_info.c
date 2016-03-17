/* vim: set ts=4 sw=4: */
/* Filename    : sysv_sem_info.c
 * Description : test SysV semaphore.
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 
 */
#define _GNU_SOURCE
#include <sys/ipc.h>
#include "ipcalsp.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

int main(int argc, char *argv[])
{
#if 0
	struct seminfo	seminfo;
	assert( semctl(0, 0, IPC_INFO, &seminfo) == 0);
	printf("* SysV semaphore : IPC_INFO\n");
	printf("  semmap = %d\n", seminfo.semmap);
	printf("  semmni = %d\n", seminfo.semmni);
	printf("  semmns = %d\n", seminfo.semmns);
	printf("  semmnu = %d\n", seminfo.semmnu);
	printf("  semmsl = %d\n", seminfo.semmsl);
	printf("  semopm = %d\n", seminfo.semopm);
	printf("  semume = %d\n", seminfo.semume);
	printf("  semusz = %d\n", seminfo.semusz);
	printf("  semvmx = %d\n", seminfo.semvmx);
	printf("  semaem = %d\n", seminfo.semaem);
#endif
	union semun	sem_un;
	struct semid_ds	sem_ds;
	memset(&sem_un, 0, sizeof(sem_un));
	memset(&sem_ds, 0, sizeof(sem_ds));
	sem_un.buf = &sem_ds;

	int sem_id, ret;
	assert((sem_id = semget(0x12340001, 0, 0)) > 0);
	printf("sem_id = %d\n", sem_id);
	if ((ret = semctl(sem_id, 0, IPC_STAT, sem_un)) != 0) {
		perror("semctl");
		return 1;
	}
	printf("* IPC_STAT for sem\n");
	printf("  ipc_perm = uid/gid/cuid/cgid/mode(%ld/%ld/%ld/%ld/%lo)\n",
			sem_ds.sem_perm.uid, sem_ds.sem_perm.gid, sem_ds.sem_perm.cuid, sem_ds.sem_perm.cgid, sem_ds.sem_perm.mode);

	sem_ds.sem_perm.mode = 0600;
	if ((ret = semctl(sem_id, 0, IPC_SET, sem_un)) != 0) {
		perror("semctl");
		return 1;
	}

	return 0;
}

