#ifndef PSU_MUTEX_H
#define PSU_MUTEX_H

#include <string>

#ifndef DEBUG
#define DEBUG 1
#endif

using namespace std;

bool psu_start_lock();
void psu_init_lock(unsigned int lockno);
void psu_mutex_lock(unsigned int lockno);
void psu_mutex_unlock(unsigned int lockno);
void psu_destroy_lock(unsigned int lockno);
void psu_stop_lock();

// helpers
string GetHostName();

#endif /* PSU_MUTEX_H */
