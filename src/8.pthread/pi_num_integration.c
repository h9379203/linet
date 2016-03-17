/* vim: set ts=4 sw=4: */
/* Filename    : pi_num_integration.c
 * Description : calculate pi
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : numerical integration method
 */

#include <stdio.h>
#include <stdlib.h>
int num_steps=400000000; /* integration 횟수: 4억번 (너무 많으면 줄이자.) */

int main()
{
	int i;
	double x, step, sum = 0.0;
	step = 1.0/(double) num_steps;
	for (i=0; i<num_steps; i++) {
		x = (i+0.5) * step;
		sum += 4.0/(1.0 + x*x);
	}
	printf("pi = %.8f (sum = %.8f)\n", step*sum, sum);
	return EXIT_SUCCESS;
}
