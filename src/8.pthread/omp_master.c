#include <stdio.h>
int main()
{
#pragma omp parallel
	{
#pragma omp master
		{
			printf("1.Hello OpenMP\n");
		}
		printf("2.Hello OpenMP\n");
	}
	/* implicit barrier */
	return 0;
}
