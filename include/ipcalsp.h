/* Filename    : ipcalsp.h
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Description :
 * Notes       :
 * 1st Release :2011-06-27 04:47:24 (Mon)
 */
#ifndef H_IPCALSP
#define H_IPCALSP

/* -------------------------------------------------------------------------- */
/* SysV IPC : shared memory */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

extern int sysv_shmget(void **ret, char *tok, key_t shm_fixkey, int size, int user_mode);
extern int sysv_shmrm(int shm_id);

/* -------------------------------------------------------------------------- */
/* SysV IPC : semaphore */
#include <sys/sem.h>
#if defined(__GNU_LIBRARY__) && !(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
    int val;                    
    struct semid_ds *buf;
    unsigned short int *array; 
    struct seminfo *__buf;
};
#endif

extern int sysv_semget(char *tok, key_t sem_fixkey, int n_sem, int sem_value, int user_mode);
extern int sysv_semrm(int sem_id);
extern int sysv_semwait(int sem_id, int sem_idx);
extern int sysv_semzwait(int sem_id, int sem_idx);
extern int sysv_sempost(int sem_id, int sem_idx);
extern int sysv_semval(int sem_id, int sem_idx);

#ifdef __USE_GNU
#include <sys/time.h>
extern int sysv_semwait_timeo(int sem_id, int sem_idx, struct timespec *ts);
#endif

/* -------------------------------------------------------------------------- */
/* SysV IPC : message queue */
extern int sysv_msgget(char *tok, key_t msg_fixkey, int user_mode);
extern int sysv_msgrm(int msg_id);


#endif
