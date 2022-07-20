#include <stdio.h>
#include "tasks.h"
#include "threading.h"

int main(){
    Row rows1[] = {{ 1000, 31 }, { 1000, 72 }, { 1000, 82 }, { 1500, 34 }, { 2000, 22 }, { 2000, 33 }};
    printf("task 1.\n");
    task1(rows1, sizeof(rows1)/sizeof(Row));
    printf("task 2.\n");
    task2(rows1, sizeof(rows1)/sizeof(Row));
    printf("task 3.\n");
    task3(rows1, sizeof(rows1)/sizeof(Row));
    return 0;
}
