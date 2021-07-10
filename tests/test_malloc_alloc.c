#include <stdio.h>

#include "malloc_helpers.h"

// Simple test that does a single allocation
#define MEM_SIZE (100)
static char mem[MEM_SIZE] __attribute__((aligned(8)));

int main() {

	os_init(mem, MEM_SIZE, FIRST_FIT);

	char * ptr = os_malloc(10);
	char * expected = (((char *) mem) + sizeof(mem_block));

	printf("Expected: %p\nFound   : %p", expected, ptr);
	if( expected != ptr ) {
		fprintf(stderr, ">>> Test failed");
	} else {
		fprintf(stderr, ">>> Test passed");
	}
}
