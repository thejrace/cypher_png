// Obarey Inc. 2018

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

// ASC sort helper
int compare(const void* a, const void* b);

// check if array contains given element
bool in_array(int val, int *arr, int size, int * found_index);

// fill given array with random integers according to given limits
void generate_random_ints(int lower, int upper, int length, int * ret_array[255]);

// scan user's console entry
char* scan_line(char *line);