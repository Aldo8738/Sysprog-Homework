#include "filetree_helpers.h"

const char * expected_output = 
	"-> get path of root:\n"
	"\n"
	"\n"
	"-> get path of 'my_dir':\n"
	"/my_dir\n"
	"\n"
	"-> get path of 'this-is-a-file.txt':\n"
	"/this-is-a-file.txt\n"
	"\n"
	"-> get path of 'file01' in directory 'my_dir':\n"
	"/my_dir/file01\n";

int main(int argc, char ** argv)
{
	/* initialize filetree testing */
	filetree_test_init();
	
	/* new tree */
	Tree * t = filetree_new();

	/* create a bunch of directories and files */
	filetree_mkdir(t->root, "my_dir");
	filetree_mkfile((Directory*)t->root->first_child, "file01", NULL, 0);
	filetree_mkfile(t->root, "this-is-a-file.txt", NULL, 0);

	/* get path */
	char * path;
	puts("-> get path of root:");
	path = filetree_get_path((Node*)t->root);
	puts(path);
	FREE(path);

	puts("");
	puts("-> get path of 'my_dir':");
	path = filetree_get_path(t->root->first_child->next);
	puts(path);
	FREE(path);

	puts("");
	puts("-> get path of 'this-is-a-file.txt':");
	path = filetree_get_path(t->root->first_child);
	puts(path);
	FREE(path);

	puts("");
	puts("-> get path of 'file01' in directory 'my_dir':");
	path = filetree_get_path(((Directory*)t->root->first_child->next)->first_child);
	puts(path);
	FREE(path);

	/* free tree */
	filetree_destroy(t);

	/* check and write results */
	filetree_test_check(argv[0], expected_output);

	return 0;
}
