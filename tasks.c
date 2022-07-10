#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "tasks.h"

/* Task 1.
Find out all the rows that sastify below conditions:
Print them to the terminal, one row per line, for example:
1000,20 1000,23 2000,16
@param nrows @param rows
The total number of rows.
The rows, for example rows[0] is the first row.
*/

typedef struct{
	pthread_t thread;
	const Row * rows;
	int nrows;
	int * res_list;
	int nres;
}Job;

#define THREADING_N 2

#define print_row(row) printf("(%d, %d)\n", (row)->a, (row)->b)

void print_job_res(Job * job){
	const Row * row_p = job->rows;
	const int * res_list = job->res_list;
	const int nres = job->nres;

	for(int i = 0; i < nres; i ++){
		print_row(&row_p[res_list[i]]);
	}
}

void * threading_filter1(void * job){
	Job * job_p = (Job *) job;
	const Row *row_p = NULL;
	int * res_list = job_p->res_list;

	for(int i = 0; i < job_p->nrows; i ++){
		row_p = &job_p->rows[i];
		if((row_p->a==1000 || row_p->a==2000|| row_p->a==3000) && (row_p->b>=10 && row_p->b<50)){
			res_list[job_p->nres ++] = i;
		}
	}

	return NULL;
}

void
task1(const Row *rows, int nrows)
{
/*
* TODO: implement this function.
*/
	int ret = 0;
	Job jobs[THREADING_N] = {0};
	Job *job_p = NULL;
	int *res_list = malloc(nrows * sizeof(int));
	int job_n_rows = ceil(((float)(nrows))/THREADING_N);

	int used_rows = 0;

	for(int i = 0; i < THREADING_N; i ++){
		job_p = &jobs[i];
		job_p->rows = rows + job_n_rows * i;
		if(used_rows + job_n_rows > nrows){
			job_p->nrows = nrows - used_rows;
		}
		job_p->nrows = job_n_rows;
		used_rows += job_p->nrows;
		job_p->res_list = res_list + job_n_rows * i;
		job_p->nres = 0;
		

		ret = pthread_create(&job_p->thread,NULL,threading_filter1, job_p);
		assert(ret == 0);
	}

	for(int i = 0; i < THREADING_N; i ++){
		job_p = &jobs[i];
		ret = pthread_join(job_p->thread,NULL);
		assert(ret == 0);
		print_job_res(job_p);
	}

FIN:
	if(NULL !=  res_list){
		free(res_list);
	}

	return;
}




/*
 * Task 2, if the array is sorted, the optimization as follows.
 *
 * */

#define LEFT_OPEN 1
#define RIGHT_OPEN  2


int get_a(const Row * row){
	return row->a;
}


int get_b(const Row * row){
	return row->b;
}

int find_first_ge(const Row *rows, int nrows, element_getter_t getter, int target){
	if(nrows == 0) {
    	return -1;
    }
    int begin = 0;
	int end = nrows;
	int mid = 0;
	const Row *row_p = NULL;

	if(getter(&rows[nrows-1]) < target){
		return -1;
	}

    while(begin < end) {
    	mid = (begin + end) / 2;

		row_p = &rows[mid];

        if(getter(row_p) < target) {
        	begin = mid + 1;
        } else {
        	end = mid; 
        }
   	}
	return begin >= nrows? -1: begin;
}

int find_last_le(const Row *rows, int nrows, element_getter_t getter, int target){
	if(nrows == 0) {
    	return -1;
    }
    int begin = 0;
    int end = nrows;
	int mid = 0;
	const Row *row_p = NULL;

	if(getter(&rows[0]) > target){
		return -1;
	}

    while(begin + 1< end) {
    	mid = (begin + end) / 2;

		row_p = &rows[mid];

        if(getter(row_p) > target) {
        	end = mid-1;
        } else {
        	begin = mid; 
        }
   	}
	return begin;
}


void
task2(const Row *rows, int nrows){
	int left_index = 0;
	int right_index = 0;

	typeof(rows->a) a_targets[] = {1000, 2000, 3000};
	int n_a_targets = sizeof(a_targets)/sizeof(int);
	for(int i = 0; i < n_a_targets; i ++){
		if(-1 == (left_index = find_first_ge(rows, nrows, get_a, a_targets[i]))){
			continue;
		}

		if(rows[left_index].a != a_targets[i]){
			continue;
		}

		if(-1 == (right_index = find_last_le(rows, nrows, get_a, a_targets[i]))){
			continue;
		}

		if(rows[right_index].a != a_targets[i]){
			continue;
		}

		printf("range: %d, [%d, %d]\n", a_targets[i], left_index, right_index);
	}
}
