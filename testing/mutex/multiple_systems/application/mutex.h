#ifndef MUTEX_H
#define MUTEX_H

#include <string>

#define DEBUG 0

using namespace std;

void enterMutex();
void exitMutex();
bool initMutex();
void destroyMutex();

// helpers
string GetHostName();

#endif /* MUTEX_H */
