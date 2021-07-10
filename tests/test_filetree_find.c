#include "filetree_helpers.h"

const char * expected_output = 
	"-> find on root:\n"
	"/\n"
	"/c\n"
	"/c/d\n"
	"/c/d/c\n"
	"/b\n"
	"/a\n"
	"/a/c\n"
	"\n"
	"-> find on c:\n"
	"/c\n"
	"/c/d\n"
	"/c/d/c\n"
	"\n"
	"-> find on root with filter 'c':\n"
	"/c\n"
	"/c/d/c\n"
	"/a/c\n";

int main(int argc, char ** argv)
{
	/* initialize filetree testing */
	filetree_test_init();
	
	/* new tree */
	Tree * t = filetree_new();

	/* create a bunch of directories and files */
	filetree_mkdir(t->root, "a");
	filetree_mkfile((Directory*)t->root->first_child, "c", NULL, 0);
	filetree_mkfile(t->root, "b", NULL, 0);
	
	filetree_mkdir(t->root, "c");
	filetree_mkdir((Directory*)t->root->first_child, "d");
	filetree_mkfile((Directory*)((Directory*)t->root->first_child)->first_child, "c", NULL, 0);
//filetree_mkfile((Directory*)((Directory*)t->root->first_child)->first_child, "p", NULL, 0);
	/* find */
	puts("-> find on root:");
	filetree_find(t->root, NULL);

	puts("");
	puts("-> find on c:");
	filetree_find((Directory*)t->root->first_child, NULL);

	puts("");
	puts("-> find on root with filter 'c':");
	filetree_find(t->root, "c");

	/* free tree */
	filetree_destroy(t);

	/* check and write results */
	filetree_test_check(argv[0], expected_output);

	return 0;
}
