#include "filetree_helpers.h"

const char * expected_output = 
	"this_is_a_directory_with_a_super_long_name     DIRECTORY\n"
	"a                                              DIRECTORY\n"
	"this_is_a_file                                 FILE\n"
	"this_is_a_directory                            DIRECTORY\n";

int main(int argc, char ** argv)
{
	/* initialize filetree testing */
	filetree_test_init();
	
	/* new tree */
	Tree * t = filetree_new();

	/* create a bunch of directories and files */
	filetree_mkdir(t->root, "this_is_a_directory");
	//filetree_mkdir((Directory*)t->root->first_child, "xd");
	filetree_mkfile(t->root, "this_is_a_file", NULL, 0);
	
	//filetree_mkdir((Directory*)t->root->first_child, "servus");
	filetree_mkdir((Directory*)t->root, "a");
	filetree_mkdir(t->root, "this_is_a_directory_with_a_super_long_name");
	//printf("\n%d\n", isSubdirectory(&t->root->node, (Directory*)t->root->first_child));
	//filetree_mkfile((Directory*)t->root->first_child, "this_is_a_file2", NULL, 0);
	
	/* ls */
	//Directory* temp = (Directory*)t->root->first_child;
	filetree_ls((Directory*)t->root);
	//filetree_rm(temp->first_child);
	//filetree_ls((Directory*)t->root->first_child);
	
	/* free tree */
	filetree_destroy(t);

	/* check and write results */
	filetree_test_check(argv[0], expected_output);

	return 0;
}
