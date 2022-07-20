#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "threading.h"
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
	THREADING_OBJ;
	const Row * rows;
	int nrows;
	int * res_list;
	int nres;
	int res_top;
}Job;

#define JOB_EMPTY(job) (0 == (job)->nres || (job)->res_top == (job)->nres)

/* how many threads do we employ, may be 2 */
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

/*
* A rough filter, sequencial scan.
* O(n)
* This filter can be a workhorse of the threads.
*/
void * filter1(void * job){
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

/*
* unordered data, let's split the Rows into pices to process with
* threading technique, so to speed up.
*/
void
task1(const Row *rows, int nrows)
{
/*
* TODO: implement this function.
*/
	int ret = 0;
	/* threading object, the first element must be THREADING_OBJ */
	Job jobs[THREADING_N] = {0};
	Job *job_p = NULL;
	/* result list, global, saves the indices of the results */
	int *res_list = malloc(nrows * sizeof(int));
	/* max rows a job thread will handle, i.e. the split size */
	int job_n_rows = ceil(((float)(nrows))/THREADING_N);

	int used_rows = 0;

	/* split Rows into pices, build Job objects we need. */
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
	}

	/* threading */
    ret = threading(jobs, THREADING_N, filter1);
	if(ret < 0){
		printf("Error while threading.\n");
		goto FIN;
	}

	/* join all */
    ret = threading_join(jobs, THREADING_N);
	if(ret < 0){
		printf("Error while join threading.\n");
		goto FIN;
	}

	/* show results, by jobs order, keep results stable */
	for(int i = 0; i < THREADING_N; i ++){
		print_job_res(jobs+i);
	}

FIN:
	/* donnot forget to release mem resouces. */
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


/* 
* Row element getter utilities,
*/
int get_a(const Row * row){
	return row->a;
}

int get_b(const Row * row){
	return row->b;
}

/*
* Find the index of the first Row which sitisfies the condition getter(Row) >= target.
* A variation of binary search.
*/
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

/*
* find the index of the last Row which sitisfies the condition getter(Row) <= target
* A variation of binary search.
*/
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

/*
* Filter use binary search.
* The arg: job should be a threading_t object.
*/
void * filter2(void * job){
	Job * job_p = (Job *)job;
	const Row *rows = job_p->rows;
	int nrows = job_p->nrows;

	const Row *sub_rows = NULL;
	int sub_nrows = 0;

	int left_index = 0;
	int right_index = 0;
	int sub_left_index = 0;
	int sub_right_index = 0;

	/* filter value == one of {1000, 2000, 3000} */
	typeof(rows->a) a_targets[] = {1000, 2000, 3000};
	int n_a_targets = sizeof(a_targets)/sizeof(int);
	for(int i = 0; i < n_a_targets; i ++){
		/* find the index range of Row.a == 1000,2000,3000 */
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

		/* printf("range: %d, [%d, %d]\n", a_targets[i], left_index, right_index); */

		/* 2nd filter, find index range matches Row.b >= 10 and Row.b < 50 */
		sub_nrows = right_index - left_index + 1;
		sub_rows = rows + left_index;

		if(-1 == (sub_left_index = find_first_ge(sub_rows, sub_nrows, get_b, 10))){
			continue;
		}

		if(-1 == (sub_right_index = find_last_le(sub_rows, sub_nrows, get_b, 50-1))){
			continue;
		}

		/* save results in job object, we will retrieve them later */
		for(int i = sub_left_index; i <= sub_right_index; i ++){
			job_p->res_list[job_p->nres++] = left_index + i;
		}
	}

	return NULL;
}

/*
* threading + binary search version.
*/
void
task2(const Row *rows, int nrows){
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
	}

    ret = threading(jobs, THREADING_N, filter2);
	if(ret < 0){
		printf("Error while threading.\n");
		goto FIN;
	}

    ret = threading_join(jobs, THREADING_N);
	if(ret < 0){
		printf("Error while join threading.\n");
		goto FIN;
	}

	for(int i = 0; i < THREADING_N; i ++){
		print_job_res(jobs+i);
	}

FIN:
	if(NULL !=  res_list){
		free(res_list);
	}

	return;
}

#define JOB_ROW(job,index)  (&(job)->rows[(index)])
#define JOB_TOP_ROW(job) (JOB_ROW((job), (job)->res_list[(job)->res_top]))
#define JOB_TOP_VALUE(getter,job) ((getter)(JOB_TOP_ROW(job)))
#define JOB_POP_VALUE(job) ((job)->res_top++)

/*
* out put results with merge sort algorithm
*/
void merge_print(Job *jobs, int njobs, element_getter_t getter){
	while(1){
		int min_index = -1;
		Job * cur_job = NULL;
		int found = 0;

		/* find row which have the min value */
		for(int i = 0; i < njobs; i ++){
			int value = 0;
			cur_job = jobs + i;
			if(! JOB_EMPTY(cur_job)){
				if(-1 == min_index){
					min_index = i;
					found = 1;
					continue;
				}
				if(JOB_TOP_VALUE(getter, cur_job) < JOB_TOP_VALUE(getter, jobs + min_index)){
					min_index = i;
					found = 1;
					continue;
				}
				
			}
		}

		if(!found){
			break;
		}
		
		print_row(JOB_TOP_ROW(jobs+min_index));
		JOB_POP_VALUE(jobs+min_index);
	}
}

/*
* threading + binary search + merge sort
*/
void
task3(const Row *rows, int nrows){
	int ret = 0;
	Job jobs[THREADING_N] = {0};
	Job *job_p = NULL;
	int *res_list = malloc(nrows * sizeof(int));
	int job_n_rows = ceil(((float)(nrows))/THREADING_N);

	int used_rows = 0;

	/* build threading objs */
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
	}

	/* threading */
    ret = threading(jobs, THREADING_N, filter2);
	if(ret < 0){
		printf("Error while threading.\n");
		goto FIN;
	}

    ret = threading_join(jobs, THREADING_N);
	if(ret < 0){
		printf("Error while join threading.\n");
		goto FIN;
	}

	/* show result, merge with Row.b */
	merge_print(jobs, THREADING_N, get_b);

FIN:
	if(NULL !=  res_list){
		free(res_list);
	}

	return;
}

