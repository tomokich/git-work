/* バイナリサーチ */
#include	<stdio.h>
#include	<stdlib.h>

int cmp_func(int *a, int *b)
{
	return(*a - *b);
}

int main()
{
	static int ary[10] = {2, 4, 1, 3, 5, 7, 9, 6, 8, 0};
	static int target = 5;
	int *ptr;
	int i;

	for(i=0; i<10; i++){
		printf("ary[%d] = %d\n", i, ary[i]);
	}
	printf("target is %d.\n", target);	
	
	qsort(ary, 10, sizeof(int), (int (*)(const void *, const void *))cmp_func);
	
	ptr = (int *)bsearch(&target, ary, 10, sizeof(int), (int (*)(const void *, const void *))cmp_func);
	if(ptr != NULL){
		printf("target is ary[%d].\n", ptr-ary);
	}else{
		printf("taget not found.\n");
	}
	
	for(i=0; i<10; i++){
		printf("ary[%d] = %d\n", i, ary[i]);
	}
	
	return(0);
}
