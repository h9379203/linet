/* Filename    : xdr_fmt.c
 * Description :
 * Notes       : 
 * 1st Release : By SunYoung Kim<sunyzero@gmail.com>, 2011-07-15 09:09:39 (Fri)
 */
#define _XOPEN_SOURCES	600
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	char *p_buf;

#if defined(__GNUC__)
# if defined(__i386__)
	/* Enable Alignment Checking on x86 */
	__asm__("pushf\norl $0x40000,(%esp)\npopf");
# elif defined(__x86_64__) 
	/* Enable Alignment Checking on x86_64 */
	__asm__("pushf\norl $0x40000,(%rsp)\npopf");
# endif
#endif

	if ((p_buf = (char *)malloc(sizeof(char) * 65536)) == NULL) {
		return EXIT_FAILURE;
	}
	memcpy(p_buf, "1234567890abc", 13);
	p_buf += 13;

	*((int *)p_buf) = 123456;
	return EXIT_SUCCESS;
}
