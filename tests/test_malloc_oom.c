#include <stdio.h>

#include "malloc_helpers.h"

// Simple test that checks out of memory behavior

#define MEM_SIZE (150)
static char mem[MEM_SIZE] __attribute__((aligned(8)));

int main() {

	os_init(mem, 150, FIRST_FIT);

	void * ptr_1 = os_malloc(100); // this should work
	void * ptr_2 = os_malloc(100); // this shouldn't

	mem_print_blocks(mem);

	int error = 0;

	printf("----\nFirst allocation:\n");
	printf("Expected: %s\n", "not NULL");
	printf("Got     : %p\n", ptr_1);

	if(!ptr_1) {
		printf("%s", "First allocation did not match expected outcome");
		error++;
	}

	printf("----\nLast allocation:\n");
	printf("Expected: %p\n", NULL);
	printf("Got     : %p\n", ptr_2);

	if(ptr_2) {
		printf("%s", "Last allocation did not match expected outcome");
		error++;
	}

	if(error) {
		fprintf(stderr, ">>> Test failed");
	} else {
		fprintf(stderr, ">>> Test passed");
	}
	exit(error);
}
