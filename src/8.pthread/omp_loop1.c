/* vim: set ts=4 sw=4: */
/* Filename    : omp_loop1.c
 * Description : simple OpenMP model
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 
 */
#include <stdio.h>
int main() 
{
	int	i;
#pragma omp parallel
#pragma omp for
	for (i=0; i<8; i++) {
		printf("[%d] Hello OpenMP\n", i);
	}
	/* implicit barrier */
	return 0;
}
