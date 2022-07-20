#include "threading.h"

int threading_(void *jobs, size_t job_size, int njobs, threading_func_t func)
{
    threading_t *threading_obj = NULL;
    void *job = NULL;
    int ret = 0;
    for(int i = 0; i < njobs; i ++){
        job = (threading_t *)(jobs + job_size * i);
        threading_obj = (threading_t *)(job);
		if((threading_obj->init_ret = pthread_create(&threading_obj->thread,NULL,func, job)) < 0){
            ret = -1;
        }
    }

    return ret;
}


int threading_join_(void *jobs, size_t  job_size, int njobs){
    threading_t *threading_obj = NULL;
    void *job = NULL;
    int ret = 0;
    for(int i = 0; i < njobs; i ++){
        job = (threading_t *)(jobs + job_size * i);
        threading_obj = (threading_t *)(job);
        if(threading_obj->init_ret != 0){
            /* skip failed threads */
            continue;
        }
		if((threading_obj->join_ret = pthread_join(threading_obj->thread,NULL)) < 0){
            ret = -1;
        }
    }

    return ret;
}