#ifndef TASKS_H
#define TASKS_H

typedef struct Row {
	int a;
	int b; 
}Row;


void task1(const Row *rows, int nrows);

typedef int (*element_getter_t)(const Row * row);

int get_a(const Row * row);
int get_b(const Row * row);
int find_first_ge(const Row *rows, int nrows, element_getter_t getter, int target);
int find_last_le(const Row *rows, int nrows, element_getter_t getter, int target);

void task2(const Row *rows, int nrows);


#endif
