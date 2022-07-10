#include <stdio.h>
#include "tasks.h"

int main(){

	Row rows1[] = {{ 1000, 31 }, { 1000, 72 }, { 1500, 12 }, { 1500, 34 }, { 2000, 22 }, { 2000, 33 }};
	task1(rows1, sizeof(rows1)/sizeof(Row));

	int index = find_first_ge(rows1, sizeof(rows1)/sizeof(Row), get_a, 2001);
	printf("index = %d\n", index);
	index = find_first_ge(rows1, 1, get_a, 2010);
	printf("index = %d\n", index);


	index = find_last_le(rows1, sizeof(rows1)/sizeof(Row), get_a, 1900);
	printf("index = %d\n", index);
	index = find_last_le(rows1, 1, get_a, 999);
	printf("index = %d\n", index);

	task2(rows1, sizeof(rows1)/sizeof(Row));

	index = find_first_ge(rows1+4, 2, get_b, 10);
	printf("%d\n", index);
	index = find_last_le(rows1+4, 2, get_b, 50-1);
	printf("%d\n", index);
	return 0;
}
