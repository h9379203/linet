#include <stdio.h>
#include <omp.h>

int main()
{

#pragma omp parallel
#pragma omp sections
	{
#pragma omp section
		{
			printf("1.Hello OpenMP\n");
		}
#pragma omp section
		{
			printf("2.Hello OpenMP\n");
			printf("2.Hello OpenMP\n");
		}
	}
	/* implicit barrier */
	return 0;
}
