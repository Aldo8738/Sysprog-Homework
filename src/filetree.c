#include <filetree.h>

/* new file tree */
Tree* filetree_new()
{
	/* TODO: create initialize new file tree */
	Tree* newTree = ALLOCATE(sizeof(Tree));
	newTree->root = ALLOCATE(sizeof(Directory));
	newTree->root->node.flags = FILE_TREE_FLAG_DIRECTORY;
	newTree->root->node.name = "";
	newTree->root->node.parent = NULL;
	newTree->root->first_child = NULL;
	return newTree;
}

/* destroy file */
void destroyFileData(File* file) {
	FREE(file->data);
}

/* destroy directory */
void destroyDirectory(Directory* dir) {
	Node* temp = dir->first_child;
	Node* next;
	while (temp != NULL){
		next = temp->next;
		if (temp->flags == FILE_TREE_FLAG_DIRECTORY ) {
			destroyDirectory((Directory*) temp);
		}
		if (temp->flags == 0) {
			destroyFileData((File*) temp);
		}
		FREE(temp->name);
		FREE(temp);
		temp = next;
	}
}

/* destroy file tree */
void filetree_destroy(Tree * tree)
{
	/* TODO: free all memory */
	if (tree == NULL) return;
	destroyDirectory(tree->root);
	FREE(tree->root);
	FREE(tree);
}

/* checks if there is a name which would be duplicated*/
unsigned int isDuplicate(Node* node, const char * name) {
	
	Node* it = node;
	while (it != NULL) {
		if (strcmp(name, it->name) == 0) {
			return 1;
		}
		it = it->next;
	}
	return 0;
}

/*Node Setter*/
void setNode(Node* node, uint32_t flags, Node* next, Node* prev, const char* name, Directory* parent) {
	node->flags = flags;
	node->next = next;
	node->prev = prev;
	node->parent = parent;
	node->name = ALLOCATE(sizeof(char) * strlen(name) + 1);
	strcpy(node->name, name);
}

/* mkdir */
FileError filetree_mkdir(Directory * parent, const char * name)
{
	if (filetree_name_valid(name) == FILE_TREE_ERROR_ILLEGAL_NAME) {
		return FILE_TREE_ERROR_ILLEGAL_NAME;
	}
		

	if (isDuplicate(parent->first_child, name) == 1) {
		return FILE_TREE_ERROR_ILLEGAL_NAME;
	}

	Directory* newDirectory = ALLOCATE(sizeof(Directory));
	newDirectory->first_child = NULL;

	if (parent != NULL) {
		
		setNode(&newDirectory->node, FILE_TREE_FLAG_DIRECTORY, parent->first_child, NULL, name, parent);
		if (parent->first_child != NULL) {
			parent->first_child->prev = &newDirectory->node;
		} 
		parent->first_child = &newDirectory->node;
		
	}

	
	//parent != NULL && parent->first_child != NULL && 
	
	return FILE_TREE_SUCCESS;
}

/* mkfile */
FileError filetree_mkfile(Directory * parent, const char * name, const void * data, size_t data_len)
{
	
	if (filetree_name_valid(name) == FILE_TREE_ERROR_ILLEGAL_NAME) 
		return FILE_TREE_ERROR_ILLEGAL_NAME;
	
	if (isDuplicate(parent->first_child, name) == 1) {
		return FILE_TREE_ERROR_DUPLICATE_NAME;
	}
	File* newFile = ALLOCATE(sizeof(File));
	newFile->data = ALLOCATE(data_len * sizeof(size_t) + 1);
	memcpy(newFile->data, data, data_len);
	newFile->data_len = data_len;
	setNode(&newFile->node, 0,  parent->first_child, NULL, name, parent);
	if (parent->first_child != NULL) {
			parent->first_child->prev = &newFile->node;
		} 
	parent->first_child = &newFile->node;

	

	return FILE_TREE_SUCCESS;
}

/*Finds size of the largest Name (of File or Directory) of the given Directory */
unsigned int largestName(const Directory* directory) {
	
	Node* it = directory->first_child;
	unsigned int largest = strlen(directory->first_child->name);
	while (it != NULL) {
		if (strlen(it->name) > largest) {
			largest = strlen(it->name);
		}
		it = it->next;
	}

	return largest;
}

/* list contents of directory, show contents of given directory  */
void filetree_ls(const Directory * dir)
{	
	if (dir == NULL || dir->first_child == NULL) return;

	unsigned int sizeofLargestName = largestName(dir) + 5;
	
	Node* it = dir->first_child;
	
	while (it != NULL) {
		if (it->flags != 0) {
			printf("%s", it->name);
			for (int i = 0; i < sizeofLargestName - strlen(it->name); i++)
				printf(" ");
			printf("DIRECTORY\n");

		} else {
			printf("%s", it->name);
			for (int i = 0; i < sizeofLargestName - strlen(it->name); i++)
				printf(" ");
			printf("FILE\n");
		}
		
		it = it->next;
	}

}

// /* find */
void filetree_find(const Directory * start, const char * name)
{
	int printAll = 0;
	Node* startNode = (Node*) start;

	// Check if it is the root to print
	if (name == NULL || strcmp(name, "") == 0) {
		printAll = 1;
	}

	if (startNode->parent == NULL && printAll == 1) {
			printf("/\n");
	} else {
		if (printAll == 1 || strcmp(startNode->name, name) == 0) {
			char* path = filetree_get_path(startNode);
			printf("%s\n", path);
			FREE(path);
		}
	}
	
	Node* it = start->first_child;
	while (it != NULL) {
		// Recursive call in case of Directory
		if (it->flags == FILE_TREE_FLAG_DIRECTORY) {
			filetree_find((const Directory*)it, name);

		} else // File (no recursion)
		{
			if (printAll == 1 || strcmp(it->name, name) == 0) {
				char* path = filetree_get_path(it);
				printf("%s\n", path);
				FREE(path);
			}
		}
		it = it->next;
	}

}

/* remove file/directory */
FileError filetree_rm(Node * node)
{
	if (node->parent == NULL) 
		return FILE_TREE_ERROR_RM_ROOT;

	Node* temp = node->next;
	Node* prev = node->prev;
	if (node == node->parent->first_child) {
		if (node->flags == FILE_TREE_FLAG_DIRECTORY){
			destroyDirectory((Directory*)node);
			
		} else {
			destroyFileData((File*)node);
		}
		
		node->parent->first_child = temp;
		FREE(node->name);
		FREE(node);
		return FILE_TREE_SUCCESS;
	}
	
	if (node != NULL) {
		if (node->flags == FILE_TREE_FLAG_DIRECTORY){
			destroyDirectory((Directory*)node);
		} else {
			destroyFileData((File*)node);
		}
	}
	
	prev->next = temp;
	

	return FILE_TREE_SUCCESS;
}

/* checks if the given node is a Subdierctory from directory*/
unsigned int isSubdirectory(Node* node, Directory* directory) {
	if (node->flags != FILE_TREE_FLAG_DIRECTORY) return 0;
	Directory* temp = (Directory*)node;
	Node* it = temp->first_child;
	while (it != NULL) {
		if (it == &directory->node) {
			return 1;
		}
		it = it->next;
	}
	return 0;
}

/* move file/directory */
FileError filetree_mv(Node * source, Directory * destination)
{

	if (source == &destination->node || isSubdirectory(source, destination) == 1) 
		return FILE_TREE_ERROR_SUBDIR_OF_ITSELF;
	

	if (isDuplicate(destination->first_child, source->name) == 1) 
		return FILE_TREE_ERROR_DUPLICATE_NAME;

	// Destination is node's parent
	if (destination == source->parent && destination->first_child == source) 
		return FILE_TREE_SUCCESS;
	
	Node* next = source->next;
	Node* prev = source->prev;
	Directory* parent = (Directory*)source->parent;

	// Node removing from curr Directory
	if (parent->first_child != source) 
	{
		if (next != NULL) next->prev = prev;
		prev->next = next;
	} else 
	{
		if (next != NULL) next->prev = NULL;
		next->parent->first_child = next;
	}

	//Node adding to destination
	if (destination->first_child != NULL) 
		destination->first_child->prev = source;
	source->next = destination->first_child;
	source->prev = NULL;
	source->parent = destination;
	destination->first_child = source;
	return FILE_TREE_SUCCESS;
}

/* print file */
void filetree_print_file(File * file)
{
	char* temp = file->data;
	for (int i = 0; i < file->data_len; i++) {
		printf("%c", *temp);
		temp++;
	}
	printf("\n");
}

/* resolve path */
FileError filetree_resolve_path(Tree * tree, const char * path, const Directory * current_dir, Node ** resulting_node)
{
	// Invalid input
	if (strcmp(path, "/..") == 0 || (tree->root == current_dir && strcmp(path, "..") == 0)) {
		return FILE_TREE_ERROR_NOT_FOUND;
	}

	// Return root
	if (path == NULL || strcmp(path, "/") == 0 || strcmp(path,"") == 0){
		Node* root = &tree->root->node;
		*resulting_node = root;
		current_dir = tree->root;
		return FILE_TREE_SUCCESS;
	}

	// This directory
	if (strcmp(path, ".") == 0) {
		Node* it = (Node*)current_dir;
		*resulting_node = it;
		current_dir = (Directory*)it;
		return FILE_TREE_SUCCESS;
	}
	//parent directory
	if (strcmp(path, "..") == 0 && current_dir->node.parent != NULL) {
		Node* it = (Node*)current_dir->node.parent;
		*resulting_node = it;
		current_dir = (Directory*)it;
		return FILE_TREE_SUCCESS;
	}

	// Finding the ultimate-target
	char* temp_2 = ALLOCATE(sizeof(char) * (strlen(path) + 1));
	strcpy(temp_2, path);
	char* ultimateTarget;
	char* next = strtok(temp_2, "/");

	while(next != NULL) {
		ultimateTarget = next;
		next = strtok(NULL, "/");
	}
	
	// Finding the target
	char* temp = ALLOCATE(sizeof(char) * (strlen(path) + 1));
	strcpy(temp, path);
	char* target = strtok(temp, "/");
	int foundTarget = 0;

	while (target != NULL) {
		Node* it = current_dir->first_child;

		if (strcmp(target, "..") == 0) {
			it = (Node*)current_dir->node.parent->first_child;
			target = strtok(NULL, "/");
			if (target != NULL) {
				if(strcmp(target, ".") == 0) {
					target = strtok(NULL, "/");
				}
			}
		}
		
		while (it != NULL) {
			if (it->name != NULL && target != NULL){
				if (strcmp(it->name, target) == 0) {
				if (it->name != NULL && ultimateTarget != NULL) {
					if (strcmp(it->name, ultimateTarget) == 0){
					foundTarget = 1;
				}
				}
				
				if (it->flags == FILE_TREE_FLAG_DIRECTORY) {
					current_dir = (Directory*)it;
				}
				*resulting_node = it;
			}
			} 
			it = it->next;
		}
		
		target = strtok(NULL, "/");
	}
	FREE(temp);
	FREE(temp_2);

	if (foundTarget == 0) {
		return FILE_TREE_ERROR_NOT_FOUND;
	}
	return FILE_TREE_SUCCESS;
}

/* get path */
char* filetree_get_path(Node * n)
{
	//calculate size of memory
	int pathLength = 0;
	Node* it = n;
	int i = 0;
	while(it != NULL) {
		pathLength += strlen(it->name) + 1;
		it = (Node*)it->parent;
		//i++;
	}

	char* result = ALLOCATE(sizeof(char) * pathLength + 1);
	char* result_2 = ALLOCATE(sizeof(char) * pathLength + 1);
	it = n;
	result[0] = '\0';
	
	while(it != NULL) {
		strcat(result, "/");
		strcat(result, it->name);
		it = (Node*)it->parent;
		
	}
	if (result != NULL) {
		strcpy(result_2, result);
	}
	
	


	//Count elements of path
	char* temp = strtok(result, "/");
	// int i = 0;
	while (temp != NULL) {
		i++;
		temp = strtok(NULL, "/");
	}
	temp = strtok(result_2, "/");
	char* arr[i + 1];
	int j = 0;
	arr[j] = temp;
	while (temp != NULL) {
		temp =  strtok(NULL, "/");
		arr[++j] = temp;
	}
	char* result_3 = ALLOCATE(sizeof(char) * pathLength + 1);
	result_3[0] = '\0';
	for (int j = i - 1; j >= 0; j--) {
		strcat(result_3, "/");
		strcat(result_3, arr[j]);
	}
	char* final_result = ALLOCATE(sizeof(char) * pathLength + 1);
	strcpy(final_result, result_3);
	FREE(result);
	FREE(result_2);
	FREE(result_3);
	return final_result;
}

/* is string valid file/directory name? */
FileError filetree_name_valid(const char * name)
{
	if (name == NULL) return FILE_TREE_ERROR_ILLEGAL_NAME;

	char temp[strlen(name)];
    strcpy(temp, name);

	//Check for invalid space and .
    if (strcmp(temp, " ") == 0  || strcmp(temp, "") == 0 || strcmp(temp, ".") == 0 || strcmp(temp, "..") == 0) {
		return FILE_TREE_ERROR_ILLEGAL_NAME;

	}
        
	int i = 0;
	//Check for invalid characters
    while (i < strlen(name)) {
        if (!((48 <= (int)temp[i] && (int)temp[i] <= 57) || (65 <= (int)temp[i] && (int)temp[i] <= 90) || (97 <= (int)temp[i] && (int)temp[i] <= 122) || (int)temp[i] == 95 || (int)temp[i] == 45 || (int)temp[i] == 46))
            return FILE_TREE_ERROR_ILLEGAL_NAME;
        i++;
    }
	
    return FILE_TREE_SUCCESS;
}

/* get error string */
const char * filetree_error_string(FileError e){
	static const char * ERROR_CODE_STRINGS[FILE_TREE_NUM_ERROR_CODES] = {
		[FILE_TREE_ERROR_NOT_FOUND]         = "no such file or directory",
		[FILE_TREE_SUCCESS]                 = "success",
		[FILE_TREE_ERROR_DUPLICATE_NAME]    = "name does already exist",
		[FILE_TREE_ERROR_ILLEGAL_NAME]		= "illegal name",
		[FILE_TREE_ERROR_SUBDIR_OF_ITSELF]  = "can't move a directory to a subdirectory of itself",
		[FILE_TREE_ERROR_RM_ROOT]           = "root can't be removed",
		[FILE_TREE_ERROR_NOT_IMPLEMENTED]   = "function not implemented yet"
	};

	if(e >= FILE_TREE_NUM_ERROR_CODES || e < 0){
		return "unknown error";
	}

	return ERROR_CODE_STRINGS[e];
}
