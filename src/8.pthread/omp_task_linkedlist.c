#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#define LIST_ITERATION	20
typedef struct lnklist LNKLIST;
struct lnklist {
	int 	num;
	LNKLIST	*next;
} *ll_head, *ll_tail;

void make_lnklist();
LNKLIST *append_lnklist(LNKLIST *l);
void walk_lnklist();

int main()
{
#pragma omp parallel
	{
#pragma omp single
		{
			make_lnklist();
			walk_lnklist();		/* 링크드 리스트를 횡단한다. */
		}
	}
	return 0;
}

void make_lnklist()
{
	int	i;
	LNKLIST	*list;
	ll_tail = ll_head = calloc(1, sizeof(LNKLIST));
	ll_head->num = -1;
	ll_head->next = ll_tail;
	for (i=0; i<LIST_ITERATION; i++) {
		list = calloc(1, sizeof(LNKLIST));
		list->num = i;
		append_lnklist(list);
	}
}

LNKLIST *append_lnklist(LNKLIST *list)
{
	ll_tail->next = list;	/* tail에 새로운 리스트 추가 */
	ll_tail = list;		/* 리스트 tail 업데이트 */
	list->next = NULL;
	return list;
}

void walk_lnklist()
{
	int 	i;
	LNKLIST	*list;
	for(i=0,list=ll_head; list != NULL; i++,list = list->next) {
#pragma omp task firstprivate(i) 
		{
#ifdef _OPENMP
			printf("[%02d : %d] (%p -> %p)\n",
					i, omp_get_thread_num(), list, list->next);
#else
			printf("[%02d] (%p -> %p)\n",
					i, list, list->next);
#endif
			sleep(1);
		}
	}
}
