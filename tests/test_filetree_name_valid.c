#include "filetree_helpers.h"

const char * expected_output = 
	"-> checking 'a':\n"
	"VALID\n"
	"\n"
	"-> checking 'File01':\n"
	"VALID\n"
	"\n"
	"-> checking 'UNDER_SCORE':\n"
	"VALID\n"
	"\n"
	"-> checking 'This-i5-a-valid_nam3.txt':\n"
	"VALID\n"
	"\n"
	"-> checking '...':\n"
	"VALID\n"
	"\n"
	"-> checking 'ill?gal':\n"
	"INVALID\n"
	"\n"
	"-> checking 'do not use whitespaces':\n"
	"INVALID\n"
	"\n"
	"-> checking '.':\n"
	"INVALID\n"
	"\n"
	"-> checking '..':\n"
	"INVALID\n"
	"\n"
	"-> checking ''(empty):\n"
	"INVALID\n";

void check_valid(const char * name)
{
	printf("-> checking '%s'", name);
	if(name[0] == '\0'){
		printf("(empty)");
	}
	puts(":");

	FileError error = filetree_name_valid(name);
	if(error == FILE_TREE_SUCCESS)
	{
		printf("VALID\n");
	}
	else if(error == FILE_TREE_ERROR_ILLEGAL_NAME)
	{
		printf("INVALID\n");
	}
	else{
		puts(filetree_error_string(error));
	}
}

int main(int argc, char ** argv)
{
	/* initialize filetree testing */
	filetree_test_init();
	
	/* name valid */
	const char * names[] = {
							/* valid */
							"a",
							"File01",
							"UNDER_SCORE",
							"This-i5-a-valid_nam3.txt",
							"...",

							/* invalid */
							"ill?gal",
							"do not use whitespaces",
							".",
							"..",
							"" /* empty filename */};

	for(int i = 0; i < sizeof(names)/sizeof(const char*); i++){
		if(i > 0)
			puts("");
		check_valid(names[i]);
	}

	/* check and write results */
	filetree_test_check(argv[0], expected_output);

	return 0;
}
