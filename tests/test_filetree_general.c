#include <assert.h>
#include "filetree_helpers.h"

const char * expected_output = 
	"-> new filetree\n"
	"\n"
	"-> creating new valid directory 'new'\n"
	"success\n"
	"\n"
	"-> creating new valid file 'text.txt' with data 'Hello there!'\n"
	"success\n"
	"\n"
	"-> creating new invalid file 'Hello there.xyz'\n"
	"illegal name\n"
	"\n"
	"-> creating same file 'text.txt' again\n"
	"name does already exist\n"
	"\n"
	"-> ls on root\n"
	"text.txt     FILE\n"
	"new          DIRECTORY\n"
	"\n"
	"-> ls on empty directory 'new'\n"
	"\n"
	"-> get path of file 'text.txt'\n"
	"/text.txt\n"
	"\n"
	"-> creating directory 'new' in directory 'new'\n"
	"success\n"
	"\n"
	"-> get path of directory 'new' in directory 'new'\n"
	"/new/new\n"
	"\n"
	"-> get path of root\n"
	"\n"
	"\n"
	"-> find on root with no filter\n"
	"/\n"
	"/text.txt\n"
	"/new\n"
	"/new/new\n"
	"\n"
	"-> find on root with filter 'new'\n"
	"/new\n"
	"/new/new\n"
	"\n"
	"-> find on '/new' with no filter\n"
	"/new\n"
	"/new/new\n"
	"\n"
	"-> find on '/new/new' with no filter\n"
	"/new/new\n"
	"\n"
	"-> resolve path '/' from root\n"
	"root\n"
	"\n"
	"-> resolve path '//new/' from root\n"
	"new\n"
	"\n"
	"-> resolve non-existent path '/old' from root\n"
	"no such file or directory\n"
	"\n"
	"-> resolve path '/new/.././text.txt' from root\n"
	"text.txt\n"
	"\n"
	"-> move '/text.txt' to '/new/'\n"
	"success\n"
	"\n"
	"-> move '/new' to '/new/new'\n"
	"can't move a directory to a subdirectory of itself\n"
	"\n"
	"-> ls on directory 'new'\n"
	"text.txt     FILE\n"
	"new          DIRECTORY\n"
	"\n"
	"-> print contents of file 'text.txt'\n"
	"Hello there!\n"
	"\n"
	"-> trying to remove root\n"
	"root can't be removed\n"
	"\n"
	"-> removing directory 'new'\n"
	"success\n"
	"\n"
	"-> ls on root\n"
	"\n"
	"-> destroy filetree\n";


int main(int argc, char ** argv)
{

	/* initialize filetree testing */
	filetree_test_init();

	FileError e;
	puts("-> new filetree");
	Tree * t = filetree_new();
	puts("");

	puts("-> creating new valid directory 'new'");
	e = filetree_mkdir(t->root, "new");
	Directory * dir_new = (Directory*)t->root->first_child;
	puts(filetree_error_string(e));
	assert(dir_new->node.flags & FILE_TREE_FLAG_DIRECTORY);
	puts("");

	puts("-> creating new valid file 'text.txt' with data 'Hello there!'");
	const char * data = "Hello there!";
	e = filetree_mkfile(t->root, "text.txt", data, strlen(data));
	File * file_text = (File*)t->root->first_child;
	puts(filetree_error_string(e));
	assert(!(file_text->node.flags & FILE_TREE_FLAG_DIRECTORY));
	puts("");

	puts("-> creating new invalid file 'Hello there.xyz'");
	e = filetree_mkfile(t->root, "Hello there.xyz", NULL, 0);
	puts(filetree_error_string(e));
	puts("");

	puts("-> creating same file 'text.txt' again");
	e = filetree_mkfile(t->root, "text.txt", NULL, 0);
	puts(filetree_error_string(e));
	puts("");

	puts("-> ls on root");
	filetree_ls(t->root);
	puts("");

	puts("-> ls on empty directory 'new'");
	filetree_ls(dir_new);
	puts("");

	puts("-> get path of file 'text.txt'");
	char * path = filetree_get_path((Node*)file_text);
	puts(path);
	FREE(path);
	puts("");

	puts("-> creating directory 'new' in directory 'new'");
	e = filetree_mkdir(dir_new, "new");
	Directory * dir_new_new = (Directory*)dir_new->first_child;
	puts(filetree_error_string(e));
	assert(dir_new_new->node.flags & FILE_TREE_FLAG_DIRECTORY);
	puts("");

	puts("-> get path of directory 'new' in directory 'new'");
	path = filetree_get_path(dir_new->first_child);
	puts(path);
	FREE(path);
	puts("");

	puts("-> get path of root");
	path = filetree_get_path((Node*)t->root);
	puts(path);
	FREE(path);
	puts("");

	puts("-> find on root with no filter");
	filetree_find(t->root, NULL);
	puts("");

	puts("-> find on root with filter 'new'");
	filetree_find(t->root, "new");
	puts("");

	puts("-> find on '/new' with no filter");
	filetree_find(dir_new, NULL);
	puts("");

	puts("-> find on '/new/new' with no filter");
	filetree_find(dir_new_new, NULL);
	puts("");

	puts("-> resolve path '/' from root");
	test_resolve(t, "/", t->root);
	puts("");

	puts("-> resolve path '//new/' from root");
	test_resolve(t, "//new/", t->root);
	puts("");

	puts("-> resolve non-existent path '/old' from root");
	test_resolve(t, "/old", t->root);
	puts("");

	puts("-> resolve path '/new/.././text.txt' from root");
	test_resolve(t, "/new/.././text.txt", t->root);
	puts("");

	puts("-> move '/text.txt' to '/new/'");
	e = filetree_mv((Node*)file_text, dir_new);
	puts(filetree_error_string(e));
	puts("");

	puts("-> move '/new' to '/new/new'");
	e = filetree_mv((Node*)dir_new, dir_new_new);
	puts(filetree_error_string(e));
	puts("");

	puts("-> ls on directory 'new'");
	filetree_ls(dir_new);
	puts("");

	puts("-> print contents of file 'text.txt'");
	filetree_print_file(file_text);
	puts("");

	puts("-> trying to remove root");
	e = filetree_rm((Node*)t->root);
	puts(filetree_error_string(e));
	puts("");

	puts("-> removing directory 'new'");
	e = filetree_rm((Node*)dir_new);
	puts(filetree_error_string(e));
	puts("");

	puts("-> ls on root");
	filetree_ls(t->root);
	puts("");

	puts("-> destroy filetree");
	filetree_destroy(t);
	t = NULL;

	/* check and write results */
	filetree_test_check(argv[0], expected_output);

	return 0;
}
