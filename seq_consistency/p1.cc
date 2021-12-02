//Program 1 of the sequential consistency check

#include <stdlib.h>
#include <stdio.h>
#include "psu_dsm.h"

int a __attribute__ ((aligned (4096)));
int b __attribute__ ((aligned (4096)));

int main(int argc, char* argv[])
{
	psu_dsm_register_datasegment(&a, 4096*2);

#if DEBUG
	// printf("%p\n", &a);
#endif

	a = 1;

	psu_dsm_free();
	
	return 0;
}
