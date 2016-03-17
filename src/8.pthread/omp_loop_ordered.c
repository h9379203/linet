/* vim: set ts=4 sw=4: */
/* Filename    : omp_loop_ordered.c
 * Description : simple OpenMP model
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 
 */
#include <stdio.h>
int main() 
{
	int	i;
#pragma omp parallel
#pragma omp for ordered
	for (i=0; i<8; i++) {
		printf("[%d] Hello OpenMP\n", i);
#pragma omp ordered
		{
			printf("\t[%d] Hello OpenMP : ordered block.\n", i);
		}
	}
	/* implicit barrier */
	return 0;
}
