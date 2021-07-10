#include <filetree.h>

#ifdef USE_MY_MALLOC
#include "malloc_helpers.h"
#define MEM_SIZE (1024)
static char mem[MEM_SIZE] __attribute__((aligned(8)));
#endif

/* initialize testing */
void filetree_test_init()
{
	#ifdef USE_MY_MALLOC
	os_init(mem, MEM_SIZE, FIRST_FIT);
	#endif

	/* turn off stdout buffering */
	setvbuf(stdout, NULL, _IONBF, 0);
}

/* test resolve */
void test_resolve(Tree * t, const char * path, Directory * current_dir){
	Node * n = NULL;
	FileError error = filetree_resolve_path(t, path, current_dir, &n);
	if(error == FILE_TREE_SUCCESS){
		if(n != NULL){
			if(n == (Node*)t->root){
				puts("root");
			}
			else{
				puts(n->name);
			}
		}
		else{
			puts("NULL");
		}
	}
	else{
		puts(filetree_error_string(error));
	}
}

/* check output and write results */
int filetree_test_check(const char * argv0, const char * expected)
{
	static char test_output_file_name[256];
	sprintf(test_output_file_name, "%s.txt", argv0);
	/* make sure all the output has been written to file */
	fflush(stdout);
	FILE * f = fopen(test_output_file_name, "rb");
	if(f == NULL){
		fprintf(stderr, "ERROR: Unable to open test output '%s'!\n", test_output_file_name);
		return 1;
	}else{
		/* get file length */
		fseek(f, 0, SEEK_END);
		long int file_len = ftell(f);
		rewind(f);
		char * file_data = malloc((file_len+1));
		fread(file_data, 1, file_len, f);
		fclose(f);
		file_data[file_len] = '\0';
		int test_passed = 0;
		if(strcmp(file_data, expected)){
			fprintf(stderr, ">>> Test failed");
		}
		else{
			test_passed = 1;
			fprintf(stderr, ">>> Test passed");
		}

		/* writing whole output */
		sprintf(test_output_file_name, "%s.results.txt", argv0);
		FILE * f = fopen(test_output_file_name, "w");
		if(f == NULL){
			fprintf(stderr, "ERROR: Unable to write final test ouput to '%s'!", test_output_file_name);
			return 1;
		}
		else{
			if(test_passed){
				fputs("TEST PASSED: No difference between EXPECTED and FOUND.\n", f);
			}
			else{
				fputs("TEST FAILED: EXPECTED differs from FOUND!\n", f);
			}
			fprintf(f,
				"######################## EXPECTED ########################\n"
				"%s\n"
				"########################## FOUND #########################\n"
				"%s\n"
				"##########################################################\n",
				expected, file_data);

			fclose(f);
		}
		free(file_data);
	}
	
	return 0;
}
