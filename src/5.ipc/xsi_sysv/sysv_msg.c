/* vim: set ts=4 sw=4: */
/* Filename    : sysv_msg.c
 * Description : message queue test code
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : usage) "./sysv_msg sender filename" , "./sysv_msg receiver 3"
 * Rel         : 2nd Ed.
 */
#include "ipcalsp.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define LEN_MQ_MTEXT	512
int msg_id;
struct mq_buf {
	long mtype;
	char mtext[LEN_MQ_MTEXT];
};

int start_msq_sender(char *srcfile);
int start_msq_receiver(long mtype);

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage : %s <sender | receiver> <filename or mtype>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((msg_id = sysv_msgget(argv[0], 0, 0664))  == -1) {
		printf("FAIL: msg_init()\n");
		exit(EXIT_FAILURE);
	}

	printf("* Message queue test program (ID:%d)\n", msg_id);
	/* Send(Server) or Recv(Client) */
	switch(argv[1][0]) {
		case 's':
			printf("+ Sender start with file(%s)\n", argv[2]);
			(void) start_msq_sender(argv[2]);
			printf("+ Finished. (Ctrl-C:Exit) (Press any key:Remove MQ)\n");
			getchar();
			break;
		case 'r':
			printf("+ Receiver start with type(%s)\n", argv[2]);
			(void) start_msq_receiver(atol(argv[2]));
			break;
		default:
			fprintf(stderr, "* Unknown option, use sender or receiver\n");
			return 1; 
	}

	sysv_msgrm(msg_id);
	return 0;
}

/* Name : start_msq_sender
 * Desc : read message from file then send msg to MessageQueue
 * Argv : char *srcfile (src message file to read)
 * Ret  : 0 (if success), -1 (fail)
 */
int start_msq_sender(char *srcfile)
{
	FILE	*fp_srcfile;
	char	rbuf[LEN_MQ_MTEXT];
	struct mq_buf mq_buf;
	int		len_mtext;

	/* open source file */
	if ((fp_srcfile = fopen(srcfile, "r")) == NULL) {
		perror("FAIL: fopen()"); 
		return -1;
	}

	while (!feof(fp_srcfile)) {
		if (fgets(rbuf, sizeof(rbuf), fp_srcfile) == NULL) { /* error or EOF */
			break; /* stop to read file */
		}

		/* read message from file, then extract mtype and mtext */
		mq_buf.mtype = (long) (rbuf[0] - '0');
		len_mtext = strnlen(rbuf, LEN_MQ_MTEXT) - 3;
		memcpy(mq_buf.mtext, (char *)&rbuf[2], len_mtext);
		printf("\t- Send (mtype:%ld,len:%d)-(mtext:%.*s)\n", 
				mq_buf.mtype, len_mtext, len_mtext, mq_buf.mtext);
		if (msgsnd(msg_id, (void *)&mq_buf, len_mtext, IPC_NOWAIT) == -1) {
			perror("FAIL: msgsnd()"); 
			break;
		}
	}
	fclose(fp_srcfile); /* close file */

	return 0;
}

/* Name : start_msq_receiver
 * Desc : read message from MessageQueue
 * Argv : long mtype (message type for reading)
 * Ret  : 0 (if success), -1 (fail)
 */
int start_msq_receiver(long mtype)
{
	int		n_recv;
	struct mq_buf mq_buf;

	while (1) {
		if ((n_recv = msgrcv(msg_id, (void *)&mq_buf, LEN_MQ_MTEXT, mtype, 0)) == -1) {
			perror("FAIL: msgrcv()"); 
			break;
		}
		printf("+ Recv (mtype:%ld,len:%d)-(%.*s)\n", 
				mq_buf.mtype, n_recv, n_recv, mq_buf.mtext);
	}
	return 0;
}
