/* Filename    : get_time0.c
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Description :
 * Notes       :
 * 1st Release :2011-06-27 04:14:42 (Mon)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/* Name : get_time0
 * Desc : print current time
 * Argv : buf, sz_buf
 * Ret  : Not null(if success), NULL(fail to convert)
 */
char * get_time0(char *buf, size_t sz_buf)
{
	/* #define STR_TIME_FORMAT      "%y-%m-%d/%H:%M:%S" */
#define STR_TIME_FORMAT     "%H:%M:%S"
	time_t  t0;
	struct tm   tm_now;

	if (buf == NULL) return NULL;
	if (time(&t0) == ((time_t)-1)) {
		return NULL;
	}
	localtime_r(&t0, &tm_now);
	if (strftime(buf, sz_buf, STR_TIME_FORMAT, &tm_now) == 0) {
		return NULL;
	}
	return buf;
}

/* Name : get_datetime0
 * Desc : print current time
 * Argv : buf, sz_buf
 * Ret  : Not null(if success), NULL(fail to convert)
 */
char * get_datetime0(char *buf, size_t sz_buf)
{
#define STR_DATETIME_FORMAT     "%y-%m-%d %H:%M:%S"
	time_t  t0;
	struct tm   tm_now;

	if (buf == NULL) return NULL;
	if (time(&t0) == ((time_t)-1)) {
		return NULL;
	}
	localtime_r(&t0, &tm_now);
	if (strftime(buf, sz_buf, STR_DATETIME_FORMAT, &tm_now) == 0) {
		return NULL;
	}
	return buf;
}
