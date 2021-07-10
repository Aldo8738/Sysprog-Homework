#ifndef FILETREE_H
#define FILETREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <os_malloc.h>

/* memory allocation macros, uncomment the following line to use your own os_malloc()/os_free() */
#define USE_MY_MALLOC
#ifdef USE_MY_MALLOC
	#define ALLOCATE(SIZE) os_malloc(SIZE)
	#define FREE(PTR) os_free(PTR)
#else
	#define ALLOCATE(SIZE) malloc(SIZE)
	#define FREE(PTR) free(PTR)
#endif

/* struct types */
typedef struct Tree Tree;
typedef struct Node Node;
typedef struct File File;
typedef struct Directory Directory;

/* node flags */
#define FILE_TREE_FLAG_DIRECTORY  0x1 /* node is a directory */
#define FILE_TREE_FLAG_EXECUTABLE 0x2 /* ***NOT USED*** is node an executable file */

/* error codes */
typedef enum{
	FILE_TREE_SUCCESS=0,
	FILE_TREE_ERROR_NOT_FOUND,
	FILE_TREE_ERROR_DUPLICATE_NAME,
	FILE_TREE_ERROR_ILLEGAL_NAME,
	FILE_TREE_ERROR_SUBDIR_OF_ITSELF,
	FILE_TREE_ERROR_RM_ROOT,
	FILE_TREE_ERROR_NOT_IMPLEMENTED,
	FILE_TREE_NUM_ERROR_CODES
}FileError;


/* Tree
 * file tree holding a reference to the root folder
 */
struct Tree{
	Directory * root;
};

/* Node
 * abstract tree node, is 'derived' by File or Directory
 * NOTE: This struct should ALWAYS be instantiated as part of either File or Directory struct.
 */
struct Node{
	uint32_t flags; /* e.g. FILE_TREE_FLAG_DIRECTORY */
	Directory * parent;
	Node * next;
	Node * prev;
	char * name;
};

/* File
 * a file in the file tree, derives from Node
 */
struct File{
	Node node; /* File 'derives' from Node */
	void * data; /* data stored in the file */
	size_t data_len; /* number of bytes in data */
};

/* Directory
 * a directory in the file tree, derives from Node
 */
struct Directory{
	Node node; /* Directory 'derives' from Node */
	Node * first_child;
};

/*
 * create new filetree with empty root folder
 * @return: new file tree
 */
Tree* filetree_new();

/*
 * clear all of the memory allocated by the tree
 * @param tree: the tree to destroy. Must not be NULL.
 */
void filetree_destroy(Tree * tree);

/*
 * create a new directory inside the given directory
 * @param parent: parent directory in which to create the new directory. Must not be NULL.
 * @param name: name of the new directory. Must not be NULL.
 * @return: error code (0 on success)
 */
FileError filetree_mkdir(Directory * parent, const char * name);

/*
 * create a new file containing arbitrary data inside the given directory
 * @param parent: parent directory in which to create the new file. Must not be NULL.
 * @param name: name of the new file. Must not be NULL.
 * @param data: the data that should be stored in the file. The data is copied into the internal struct. Can be set to NULL for an empty file.
 * @param data_len: number of bytes for the given data
 * @return: error code (0 on success)
 */
FileError filetree_mkfile(Directory * parent, const char * name, const void * data, size_t data_len);

/*
 * print all files and directories contained by the given directory to stdout
 * @param dir: directory of which to show the content. Must not be NULL.
 */
void filetree_ls(const Directory * dir);

/*
 * traverse the tree preorder from given start node, printing absolute path of all files and directories recursively, including the start node, to stdout.
 * @param start: start directory from which to start traversing. Must not be NULL.
 * @param name: filter output by files/directories matching exactly the given name. Can be empty or set to NULL to deactivate filtering. No support for wildcards (*)
 */
void filetree_find(const Directory * start, const char * name);

/* move a File or Directory from one location to another
 * @param source: the File/Directory to move. Must not be NULL.
 * @param destination: new parent Directory to which the source should be moved. Must not be NULL.
 * @return: error code (0 on success)
 */
FileError filetree_mv(Node * source, Directory * destination);

/*
 * remove a file/directory recursively
 * @param node: file/directory to remove
 * @return: error code (0 on success)
 */
FileError filetree_rm(Node * node);

/* 
 * print contents of a file, interpreted as chars, to stdout, a linebreak is printed at the end
 * @param file: the file to print. Must not be NULL.
 */
void filetree_print_file(File * file);

/*
 * get the corresponding node from a given path
 * @param tree: the file tree of which to get corresponding node from
 * @param path: the relative or absolute path to resolve (e.g. /home/documents/file1.txt or ./file2.txt). "/" or the empty string "" corresponds to the root. Must not be NULL.
 * @param current_dir: current directory to use if a relative path is given (that is, path does not start with '/'), else this paramater is ignored. Must not be NULL.
 * @param n: Node corresponding to the given path. If no such file or directory was found, *n remains unchanged. NULL can be passed (this may be useful if you just want to test whether a file/directory exists).
 * @return: error code (0 on success)
 */
FileError filetree_resolve_path(Tree * tree, const char * path, const Directory * current_dir, Node ** n);

/*
 * get full absolute path of given node without an ending '/' (e.g. path of the directory 'abc' in root folder is '/abc')
 * filetree_get_path() on root returns an empty string ""
 * @param n: node to get absolute path from. Must not be NULL.
 * @return: newly allocated memory location containing the path as a 0-terminated string. Use FREE(X) to free the memory.
 */
char* filetree_get_path(Node * n);

/*
 * check if a given name is valid
 * @name: name to check for illegal characters. Must not be NULL.
 * @return: error code, ILLEGAL_NAME if name contains not allowed characters or is empty
 */
FileError filetree_name_valid(const char * name);

/*
 * get string corresponding to given error code
 * @param e: error code
 * @return: string describing the given error
 */
const char* filetree_error_string(FileError e);

unsigned int isSubdirectory(Node* node, Directory* destination);

#endif
