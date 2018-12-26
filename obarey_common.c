// Obarey Inc. 2018

#include "obarey_common.h";

int compare(const void* a, const void* b) {
	int int_a = *((int*)a);
	int int_b = *((int*)b);

	if (int_a == int_b) return 0;
	else if (int_a < int_b) return -1;
	else return 1;
}

bool in_array(int val, int *arr, int size, int * found_index) {
	int i;
	for (i = 0; i < size; i++) {
		if (arr[i] == val) {
			*found_index = i;
			return true;
		}
	}
	return false;
}

void generate_random_ints(int lower, int upper, int length, int * ret_array[255]) {
	int i;
	for (i = 0; i < length; i++) {
		int num = (rand() % (upper - lower + 1)) + lower;
		ret_array[i] = num;
	}
}

char* scan_line(char *line) {
	int ch; // as getchar() returns `int`
	if ((line = malloc(sizeof(char))) == NULL) { //allocating memory
		//checking if allocation was successful or not
		printf("unsuccessful allocation");
		exit(1);
	}
	line[0] = '\0';
	for (int index = 0; ((ch = getchar()) != '\n') && (ch != EOF); index++){
		if ((line = realloc(line, (index + 2) * sizeof(char))) == NULL)
		{
			//checking if reallocation was successful or not
			printf("unsuccessful reallocation");
			exit(1);
		}
		line[index] = (char)ch; //type casting `int` to `char`
		line[index + 1] = '\0'; //inserting null character at the end
	}
	return line;
}