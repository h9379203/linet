/* vim: set ts=4 sw=4: */
/* Filename    : lib_sysv_shm.c
 * Description : Shared Memory is the fastest method among IPCs.
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : SHM = Shared Memory
 */
#include "ipcalsp.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

int sysv_shmget(void **ret, char *tok, key_t shm_fixkey, int size, int user_mode);
int sysv_shmrm(int shm_id);

/* Name : shm_init
 * Desc : init shared memory
 * Argv : tok(path), shm_fixkey(key), int size(byte), user_mode(octal notation)
 * Ret  : shm ID (if success), -1 (if fail)
 *        ret (return address)
 */
int sysv_shmget(void **ret, char *tok, key_t shm_fixkey, int size, int user_mode)
{
	key_t	shm_key;
	int		shm_id;
	char 	buf_err[128];

	if (ret == NULL || size < 0) { /* error */
		return -1;
	}

	if (tok != NULL) {
		if ((shm_key = ftok(tok, 1234)) == -1) {
			return -1;
		}
	} else {
		shm_key = shm_fixkey;  /* Fixed IPC key */
	}

	if ((shm_id = shmget(shm_key, size, IPC_CREAT|IPC_EXCL|user_mode)) == -1) {
		if (errno == EEXIST) { /* already exist shm, just get ipc id */
			shm_id = shmget(shm_key, 0, 0);
		}
	}
	if (shm_id == -1) {
		strerror_r(errno, buf_err, sizeof(buf_err));
		fprintf(stderr,"FAIL: shmget():%s [%d]\n", buf_err, __LINE__);
		return -1;
	}

	if ((*ret = shmat(shm_id, 0, 0)) == (void *)-1) { /* error */
		strerror_r(errno, buf_err, sizeof(buf_err));
		fprintf(stderr,"FAIL: shmat():%s [%d]\n", buf_err, __LINE__);
		return -1;
	}

	return shm_id;
}

/* Name : sysv_shmrm
 * Desc : remove shared memory
 * Argv : int shm_id or key
 * Ret  : 0 (if success), -1 (fail)
 */
int sysv_shmrm(int shm_id)
{
	int		ret;
	char	buf_err[128];
	if ((ret = shmctl(shm_id, IPC_RMID, NULL)) == -1) {
		strerror_r(errno, buf_err, sizeof(buf_err));
		fprintf(stderr,"FAIL: shmctl():%s [%d]\n", buf_err, __LINE__);
		return -1;
	}
	return ret;
}
