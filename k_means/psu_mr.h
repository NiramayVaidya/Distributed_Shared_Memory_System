#ifndef PSU_MR_H
#define PSU_MR_H

#ifndef DEBUG
#define DEBUG 1
#endif

#define USE_DSM 0
#define USE_MULTIPLE_INTERMEDIATE 1

void psu_mr_setup(unsigned int tid, unsigned int nthreads);
void psu_mr_map(void (*map_fp)(void *, void *), void *indata, void *outdata);
void psu_mr_reduce(void (*reduce_fp)(void *, void *), void *indata, void *outdata);
void psu_mr_destroy();

void map_kmeans(void *indata, void *outdata);
void reduce_kmeans(void *indata, void *outdata);

#endif /* PSU_MR_H */
