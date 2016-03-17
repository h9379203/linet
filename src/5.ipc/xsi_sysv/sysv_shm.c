/* vim: set ts=4 sw=4: */
/* Filename    : sysv_shm_info.c
 * Description : IPC_INFO
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 
 */
#define _GNU_SOURCE
#include <sys/shm.h>
#include "ipcalsp.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#define SZ_SHM_SEGMENT		4096

int main()
{
	int		shm_id;
	char 	*shm_ptr;
	int		n_read = 0; size_t	n_input = 128;
	char	*p_input = (char *) malloc(n_input);

	printf("c : Create shared memory without key.\n");
	printf("number : attach shared memory with IPC id number.\n>>");

	if ( (n_read = (int) getline(&p_input, &n_input, stdin)) == -1) { /* error */
		return -1;
	}
	if (p_input[0] == 'c') {
		shm_id = sysv_shmget((void **)&shm_ptr, NULL, IPC_PRIVATE, SZ_SHM_SEGMENT, 0664);
		if (shm_id == -1) {
			fprintf(stderr, "FAIL: sysv_shmget()\n");
			exit(EXIT_FAILURE);
		}
	} else {
		shm_id = atoi(p_input);
		if ((shm_ptr = (char *)shmat(shm_id, 0, 0)) == (char *)-1) {
			fprintf(stderr, "FAIL: shmat()\n"); 
			exit(EXIT_FAILURE);
		}
	}
	printf("* SHM IPC id(%d), PID(%d)\n", shm_id, getpid());

	printf("'*' Print current shm.\n'.' Exit. Otherwise input text to shm.\n");
	printf("'?' print shm info\n");
	while (1) {
		printf("\n>>");
		if ( (n_read = (int) getline(&p_input, &n_input, stdin)) == -1) { /* error */
			return -1;
		}
		if (p_input[0] == '.') {
			break;
		} else if (p_input[0] == '?') {
			struct shmid_ds shm_ds;
			if (shmctl(shm_id, IPC_STAT, &shm_ds) == -1) {
				printf("FAIL: shmctl()\n");
			} else {
				printf("size(%ld) # of attach(%ld)\n", shm_ds.shm_segsz, shm_ds.shm_nattch);
				printf("shm_cpid(%d) shm_lpid(%d)\n", shm_ds.shm_cpid, shm_ds.shm_lpid );
			}
#if 0
			/* 공유 메모리 정보 */
			struct shminfo shm_info;
			if (shmctl(shm_id, IPC_INFO, (struct shmid_ds *)&shm_info) == -1) {
				printf("FAIL: shmctl()\n");
			} else {
				printf("SHM: shmmni(%ld) shmmax(%ld) shmall(%ld)\n", 
						shm_info.shmmni, shm_info.shmmax, shm_info.shmall);
			}
#endif
		} else if (p_input[0] == '*') {
			printf("shm -> '%.*s'\n", SZ_SHM_SEGMENT, shm_ptr);
		} else {
			memcpy(shm_ptr, p_input, n_read);
		}
	}

	printf("* Would you remove shm (IPC id : %d) (y/n)", shm_id);
	if ( (n_read = (int)getline(&p_input, &n_input, stdin)) == -1) { /* error */
		return -1;
	}
	if (p_input[0] == 'y') {
		sysv_shmrm(shm_id);	/* remove shm */
	}

	return 0;
}
