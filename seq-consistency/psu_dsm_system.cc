#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "psu_dsm_system.h"

#define PAGE_SIZE (sysconf(_SC_PAGESIZE))

static void segv_handler(int signum, siginfo_t *info, void *ucontext) {
	printf("Received signal: %d(SIGSEGV)\n", signum);
#if DEBUG
	printf("%p\n", info->si_addr);
#endif
	mprotect(info->si_addr, PAGE_SIZE, PROT_WRITE);
}

static void register_segv_handler() {
	struct sigaction sa;
 	memset(&sa, 0, sizeof(struct sigaction));
 	sa.sa_sigaction = &segv_handler;
	sa.sa_flags = SA_SIGINFO;
 	sigaction(SIGSEGV, &sa, NULL);
}

void psu_dsm_register_datasegment(void *psu_ds_start, size_t psu_ds_size) {
	register_segv_handler();
	mprotect(psu_ds_start, psu_ds_size, PROT_NONE);
}
