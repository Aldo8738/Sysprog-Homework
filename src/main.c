#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <filetree.h>
#include <os_malloc.h>

#ifdef USE_MY_MALLOC
#define MEM_SIZE (1024)
static char mem[MEM_SIZE] __attribute__((aligned(8)));
#endif


#define MAX_STR 255

// color escape sequences
#define GRN "\e[1;32m"
#define BLU "\e[1;34m"
#define RESET "\x1B[0m"

// hash codes
#define help 6385292014
#define quit 6385632776

#define ls 5863588
#define mkdir 210720772860
#define mkfile 6953785576157
#define cd 5863276
#define rm 5863780
#define mv 5863624
#define print 210724587794
#define find 6385224550

void intro();
void terminal_line();

void help_f();
void quit_f();

void filetree_ss();
void add_dir_str(char* str);
void remove_last_dir();
void name_error(char* cmd_name, char* name);
void path_error(char* cmd_name, char* path);
Node* func_find(char *name);

int get_input(char input[]);

const unsigned long hash(const char*str){
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;
	return hash;
}

char* loc = "main";
char dir[256] = "/";
Tree* t = NULL;
Directory* cur_dir = NULL;

int main(int argc, char* argv[]){
	#ifdef USE_MY_MALLOC
	os_init(mem, MEM_SIZE, FIRST_FIT);
	#endif
	loc = "filetree";
	filetree_ss();
}

int get_input(char input[]){
	char line[MAX_STR];
	char *pos;
	int length;
	terminal_line();
	if(fgets(line, MAX_STR, stdin)!=NULL){
		if((pos = strchr(line, '\n')) != NULL)
			*pos = '\0';

		length = strlen(line);
		strncpy(input, line, length);
		input[length] = '\0';
		return 0;
	}
	return -1;
}

void terminal_line(){
	printf(GRN"dos@%s"RESET":"BLU"%s"RESET"$ ", loc, dir);
}

void add_dir_str(char* str){
	int len = strlen(dir);
	dir[len] = '/';
	strcpy(&dir[len+1], str);
}

void remove_last_dir(){
	char* pos = strrchr(dir, '/');
	if(!pos)
		return;

	dir[pos-dir] = '\0';

}

char* status = "SUCCESS";
char* percent = "99";

void intro(){
	printf(
	"            _______________________    \n"       
	"           |\\____________________/|\\ \n"
	"           ||                     || \\ \n"
	"           || "GRN"> LOADING DOS OS "RESET"   ||  \\ \n"
	"           ||                     ||   | \n"
	"           || "GRN"> [             ]"RESET"   ||   | \n"
	"           || "GRN">  %s%% COMPLETE"RESET"     ||   | \n"
	"           || "GRN">  %s"RESET"          ||   | \n"
	"           ||_____________________|| _// \n"
	"           |/_____________________\\|_// \n"
	"            __\\___________________// \n"
	"         __/e*/**/**/**/?*<#//***//||\n"
	"        /#***/#######/****#//***//_||\n"
	"\n", percent, status);
	printf(GRN"TERMINAL START\n");
	printf("Type 'quit' to quit and 'help' to see available commands"RESET"\n");
}

void help_f(){
	printf(
	"available commands:\n"
	"ls                           cd [path]\n" 		
	"rm [path]                    mkfile [name]\n"
	"print [filename]             mv [source] [destination]\n"
	"find [filter name]           mkdir [name]\n");	
}

void quit_f(){
	printf("quitting\n");
	filetree_destroy(t);
	exit(0);
}

void fat_ss(){
	puts("fat");
}

void name_error(char *cmd_name, char* arg){
	if(!arg)
		printf("usage: %s, [%s]\n", cmd_name, arg);
	
}

void path_error(char* cmd_name, char* path){
	if(!path)
		printf("usage: %s, [path], path is absolute or relative, consists of directory names separated by '/'\n", cmd_name);
}


Node* func_find(char* name){
	Node* node_to_find = NULL;
	FileError e;
	e = filetree_resolve_path(t, name, cur_dir, &node_to_find);
	if(e != FILE_TREE_SUCCESS){
		puts(filetree_error_string(e));
		return NULL;
	}
	return node_to_find;
	
}

void print_on_error(FileError e)
{
	if(e != FILE_TREE_SUCCESS){
		puts(filetree_error_string(e));
	}
}

void filetree_ss(){
	char input[MAX_STR];
	t = filetree_new();

	if(!t){
		status = "FAILURE";
		percent = "00";
		intro();
		printf("filetree_new() not implemented, exiting...\n");
		exit(1);
	}
	intro();

	cur_dir = t->root;
	FileError e;
        while(get_input(input) == 0){
	        char* command = strtok(input, " ");
		char* arg = strtok(NULL, " ");

	        if(!command) continue;

	        switch(hash(command)){
			case help:
				help_f();
				continue;
			case quit:
				quit_f();
				continue;
			case mkdir:
			        name_error("mkdir", "directory name");
					e = filetree_mkdir(cur_dir, arg);
			        print_on_error(e);
			        continue;
			case rm:
			        path_error("rm", arg);

				Node* node_to_remove = func_find(arg);

				if(!node_to_remove)
					continue;

			        e = filetree_rm(node_to_remove);
					print_on_error(e);
			        continue;
			case mv:
				if(!arg){
					printf("usage: mv [source] [destination]\n");
					continue;
				}
				char* dest_ = strtok(NULL, " ");
				if(!dest_){
					printf("usage: mv [source] [destination]\n");
					continue;
				}

				Node* source = func_find(arg);
				Node* dest = func_find(dest_);
				
				if(!(source && dest)){
					continue;
				}

				e = filetree_mv(source, (Directory*) dest);
				print_on_error(e);
				continue;
			case print:
				if(!arg){
					printf("usage: print [file]\n");
					continue;
				}
				Node * file_node = NULL;
				e = filetree_resolve_path(t, arg, cur_dir, &file_node);
				if(e != FILE_TREE_SUCCESS){
					puts(filetree_error_string(e));
				}
				else if(file_node->flags & FILE_TREE_FLAG_DIRECTORY){
					printf("'%s' is a directory\n", arg);
				}
				else{
					filetree_print_file((File*)file_node);
				}
				continue;	
			case cd:
			        path_error("cd", arg);
			        char* path = arg;

			        Node* path_node = NULL;
                                e = filetree_resolve_path(t, path, cur_dir, &path_node);

			        if(path_node) cur_dir = (Directory*) path_node;
			        
					print_on_error(e);
				if(e == FILE_TREE_SUCCESS){
					char * absolute_path = filetree_get_path(path_node);
					if(absolute_path[0] == '\0'){
						dir[0] = '/';
						dir[1] = '\0';
					}
					else{
						strcpy(dir, absolute_path);
					}

					FREE(absolute_path);
				}
			        continue;

		        case mkfile:
				name_error("mkfile", "name");
	 			char *data = strtok(NULL, "\"");
				if(!data){
					printf("usage: mkfile [name] \"[data]\"\n");
					continue;
				}

				e = filetree_mkfile(cur_dir, arg, data, strlen(data));
			        continue;

		        case ls:
		       	        filetree_ls(cur_dir);
			        continue;
			case find:
		 		filetree_find(cur_dir, arg);
				continue;		
		        default:
			        printf("unknown command\n");
			        continue;
	       }
	      
	
       }	       
}

void filesystem_ss(){
	puts("filesystem");
}


