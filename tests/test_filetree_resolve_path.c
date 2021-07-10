#include "filetree_helpers.h"

const char * expected_output = 
	"-> resolve root with '/' from root:\n"
	"root\n"
	"\n"
	"-> resolve root with '' from root:\n"
	"root\n"
	"\n"
	"-> resolve directory 'a' with '/a/' from root:\n"
	"a\n"
	"\n"
	"-> resolve directory 'a' with '.' from 'a':\n"
	"a\n"
	"\n"
	"-> resolve file 'b' with '/a//b' from root:\n"
	"b\n"
	"\n"
	"-> resolve file 'c' with './../c' from 'a':\n"
	"c\n"
	"\n"
	"-> generate 'not found' error with '/a/b/c' from root:\n"
	"no such file or directory\n"
	"\n"
	"-> generate 'not found' error with '..' from root:\n"
	"no such file or directory\n";


int main(int argc, char ** argv)
{
	/* initialize filetree testing */
	filetree_test_init();

	/* new tree */
	Tree * t = filetree_new();

	/* create a bunch of directories and files */
	filetree_mkdir(t->root, "a");
	filetree_mkfile((Directory*)t->root->first_child, "b", NULL, 0);
	filetree_mkfile(t->root, "c", NULL, 0);

	/* resolve path */
	puts("-> resolve root with '/' from root:");
	test_resolve(t, "/", t->root);

	puts("");
	puts("-> resolve root with '' from root:");
	test_resolve(t, "", t->root);

	puts("");
	puts("-> resolve directory 'a' with '/a/' from root:");
	test_resolve(t, "/a/", t->root);

	puts("");
	puts("-> resolve directory 'a' with '.' from 'a':");
	test_resolve(t, ".", (Directory*)t->root->first_child->next);

	puts("");
	puts("-> resolve file 'b' with '/a//b' from root:");
	test_resolve(t, "/a//b", t->root);

	puts("");
	puts("-> resolve file 'c' with './../c' from 'a':");
	test_resolve(t, "./../c", (Directory*)t->root->first_child->next);

	puts("");
	puts("-> generate 'not found' error with '/a/b/c' from root:");
	test_resolve(t, "/a/b/c", t->root);

	puts("");
	puts("-> generate 'not found' error with '..' from root:");
	test_resolve(t, "..", t->root);


	/* free tree */
	filetree_destroy(t);

	/* check and write results */
	filetree_test_check(argv[0], expected_output);

	return 0;
}
