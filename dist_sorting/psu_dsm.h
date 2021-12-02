#ifndef PSU_DSM_H
#define PSU_DSM_H

#include <stdlib.h>
#include <unistd.h>

#ifndef DEBUG
#define DEBUG 1
#endif

#define PAGE_SIZE (sysconf(_SC_PAGESIZE))
#define PAGE_DOWN(addr) ((addr) & ~(PAGE_SIZE - 1))

bool initDir();
void stopDir();

void psu_dsm_register_datasegment(void *psu_ds_start, size_t psu_ds_size);
void psu_dsm_free();

#endif /* PSU_DSM_H */
