#include <stdio.h>
int main()
{
#pragma omp parallel
	{
#pragma omp single
		{
			printf("1.Hello OpenMP\n");
		}
		/* implicit barrier */
		printf("2.Hello OpenMP\n");
	}
	/* implicit barrier */
	return 0;
}
