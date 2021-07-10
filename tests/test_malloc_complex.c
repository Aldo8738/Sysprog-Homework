#include <stdio.h>

#include "malloc_helpers.h"

// More complex test that does multiple allocation and frees
// In the end everything should be properly free'd

#define MEM_SIZE ((size_t)(1024 * 1024)) // 1MB
static char mem[MEM_SIZE] __attribute__((aligned(8)));

int main() {

	os_init(mem, MEM_SIZE, FIRST_FIT);


	void * ptr1 = os_malloc(100);

	void * ptr2 = os_malloc(1024 * 512);

	void * ptr3 = os_malloc(1);

	mem_print_blocks(mem);

	os_free(ptr2);

	os_free(ptr1);
	
	os_free(ptr3);
	printf("\nfirst part:\n");
	mem_print_blocks(mem);


	////////////////////////////////
	printf("\nSecond part: \n");
	ptr1 = os_malloc(100);
	ptr2 = os_malloc(100);
	ptr3 = os_malloc(100);

	mem_print_blocks(mem);

	os_free(ptr2);
	//
		printf("\nfree ptr2:\n\n");
		
	//

	mem_print_blocks(mem);

	ptr2 = os_malloc(90);

	//
	printf("90 alloc");
	//
	
	mem_print_blocks(mem);
	printf("\nPart 3\n");
	
	os_free(ptr1);
	printf("free ptr1: \n");
	mem_print_blocks(mem);
	os_free(ptr2);
	
	
	mem_print_blocks(mem);
	os_free(ptr3);
	
	mem_print_blocks(mem);

	int error = 0;

	size_t final_free_space = mem_size_free(mem);
	printf("Initial memory   : %zd\n", MEM_SIZE);
	printf("Final free memory: %zd\n", final_free_space);
	if(final_free_space != MEM_SIZE) {
		fprintf(stderr,"Error: Final free memory did not match initial size\n");
		error++;
	}

	unsigned int final_block_count = mem_block_count(mem);
	printf("Expected block count: %u\n", 1);
	printf("Final block count   : %u\n", final_block_count);
	if(final_block_count != 1) {
		fprintf(stderr, "Final block count did not match expected count\n");
		error++;
	}

	if(error) {
		fprintf(stderr, ">>> Test failed");
	} else {
		fprintf(stderr, ">>> Test passed");
	}
	exit(error);
}
