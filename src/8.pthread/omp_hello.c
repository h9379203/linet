/* vim: set ts=4 sw=4: */
/* Filename    : omp_hello.c
 * Description : simple OpenMP model
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 
 */
#include <stdio.h>
#include <omp.h>
int main() 
{
#pragma omp parallel num_threads(4)
	{
		printf("Hello OpenMP\n");
	}
	return 0;
}
