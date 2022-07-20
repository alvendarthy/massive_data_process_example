#ifndef _THREADING_H_
#define _THREADING_H_

#include <pthread.h>

/* for parallel */
typedef struct{
    pthread_t thread;
    int init_ret;
    int join_ret;
}threading_t;

#define THREADING_OBJ threading_t threading;

typedef void * (*threading_func_t)(void * job);

int threading_(void *jobs, size_t job_size, int njobs, threading_func_t func);

#define threading(jobs,njobs,func) threading_((jobs), sizeof(typeof(*(jobs))), (njobs), (func))

int threading_join_(void *jobs, size_t  job_size, int njobs);

#define threading_join(jobs,njobs) threading_join_((jobs), sizeof(typeof(*(jobs))), (njobs))

#endif