/* Filename    : xdr_fmt2.c
 * Description :
 * Notes       : 
 * 1st Release : By SunYoung Kim<sunyzero@gmail.com>, 2011-07-15 09:09:39 (Fri)
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct my_elem {
	char	str[13];
	long	l;
} MY_ELEM;

int main() {
	MY_ELEM *p_buf;

#if !defined(__GNUC__)
# if defined(__i386__)
	/* Enable Alignment Checking on x86 */
	__asm__("pushf\norl $0x40000,(%esp)\npopf");
# elif defined(__x86_64__) 
	/* Enable Alignment Checking on x86_64 */
	__asm__("pushf\norl $0x40000,(%rsp)\npopf");
# endif
#endif

	if ((p_buf = (struct my_elem *)malloc(65536)) == NULL) {
		return EXIT_FAILURE;
	}
	memcpy(p_buf->str, "1234567890abc", 13);
	p_buf->l = 123456;
	return EXIT_SUCCESS;
}
