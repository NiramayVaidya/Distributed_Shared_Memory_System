// Program 1 of the sequential consistency check

#include <stdlib.h>
#include <stdio.h>
#include "psu_dsm.h"

int main(int argc, char* argv[]) {
	int *a = (int *) psu_dsm_malloc((char *) "a", sizeof(int));
	// int *b = (int *) psu_dsm_malloc((char *) "b", sizeof(int));
	// int *a = (int *) psu_dsm_malloc((char *) "a", 4096);
	// int *b = (int *) psu_dsm_malloc((char *) "b", 4096);

#if DEBUG
	// printf("%p\n", a);
#endif

	*a = 1;

	psu_dsm_free();
	
	return 0;
}
