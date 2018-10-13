#ifndef PROJECT_HEADER
#define PROJECT_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#include <sys/mman.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void pipefun(char text[], char searchstring[]);
void socketfun(char text[], char searchstring[]);
void sharedmemfun(char text[], char searchstring[]);

#endif
