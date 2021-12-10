// Program 2 of the sequential consistency check

#include <stdlib.h>
#include <stdio.h>
#include "psu_dsm.h"

int main(int argc, char* argv[]) {
	int *a = (int *) psu_dsm_malloc((char *) "a", sizeof(int));
	int *b = (int *) psu_dsm_malloc((char *) "b", sizeof(int));
	// int *a = (int *) psu_dsm_malloc((char *) "a", 4096);
	// int *b = (int *) psu_dsm_malloc((char *) "b", 4096);
	
	while (*a != 1);
	*b = 1;

	printf("a = %d\n", *a);

	psu_dsm_free();

	return 0;
}
