#include "structure.h"

/*
 *	Tree functions
 */

// Tree operations

fsNode* createFSNode(void) {
	fsNode* node = (fsNode*)malloc(sizeof(fsNode));
	if ( node == NULL ) {
		printf("Failed to allocate memory!\n");
		exit(EXIT_FAILURE);
	}
	
	node->name = NULL;
	node->children = NULL;

	return node;
}

void initFileSystem(fsTree* fs) {
	fsNode* entry = createFSNode();
	entry->name = strdup("/home/Sastra");
	fs->root = entry;
	createFSTree(fs->root);
}

void deallocFileSystem(fsNode* node) {
	if (node == NULL)
		return;

	int tmp_count = countDirElmt(node->name);
	if (tmp_count < 0) 
		return;
	if (node->children != NULL) {
		for (int i = 0; i < tmp_count; i++) {
			if (node->children[i] != NULL) {
			    deallocFileSystem(node->children[i]);
			}
		}
		free(node->children);
	}
	if (node->name != NULL) 
		free(node->name);
	free(node);
}

void createFSTree(fsNode* node) {
	int tmp_count = countDirElmt(node->name);
	if (tmp_count < 0) 
		return;
	char* tmp_path = strdup(node->name);
	
	if ( isRegFile(node->name) == 0 )
		getDirElmt(node);
	if ( node->children == NULL) {
		free(tmp_path);
		return;
	}
	
	for (int i = 0; i < tmp_count; i++) {
		if ( node->children[i] != NULL && isRegFile(node->children[i]->name) == 0 )
			createFSTree(node->children[i]);
		printf("File system element created! %s\n", node->children[i]->name);
	}
	free(tmp_path);
}

// Main file system operations

int countDirElmt(char* path) {
	int len = 0;
	DIR* dir;
	dir = opendir(path);
	if ( dir == NULL )
		return 0; 

	struct dirent* entry;
	while ( (entry = readdir(dir)) != NULL ) {
		if ( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && entry->d_name[0] != '.')
			len++;
	}
	closedir(dir);

	return len;
}

void getDirElmt(fsNode* node) {
	int tmp_count = countDirElmt(node->name);
	if (tmp_count <= 0) {
		node->children = NULL;
		return;
	}

	node->children = (fsNode**)malloc(sizeof(fsNode*)*tmp_count);
	if (node->children == NULL) {
		printf("Failed to allocate memory!\n");
		exit(EXIT_FAILURE);
	}

	DIR* dir = opendir(node->name);
	if (dir == NULL) {
		free(node->children);
		node->children = NULL;
		return;
	}

	struct dirent* entry;
	int i = 0;
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_name[0] == '.') 
			continue;
		if (i >= tmp_count) 
			break;

		size_t path_len = strlen(node->name) + strlen(entry->d_name) + 2;
		char* tmp_path = malloc(path_len);
		if (strcmp(node->name, "/") == 0) 
			snprintf(tmp_path, path_len, "/%s", entry->d_name);
		else 
			snprintf(tmp_path, path_len, "%s/%s", node->name, entry->d_name);

		fsNode* pBuf = createFSNode();
		pBuf->name = tmp_path;  

		node->children[i] = pBuf;
		i++;
	}
	closedir(dir);
}

int pathExists(char* path) {
	if ( access(path, F_OK != -1) ) 
		return 0;
	else
		return 1;
} 

int dirPermissions(char* path) {
	if ( access(path, R_OK != -1) && access(path, W_OK != -1) ) {
		printf("File is readable and writable.\n");
		return 0;
	} else if ( access(path, R_OK != -1) ) {
		printf("File is readable.\n");
		return 1;
	} else {
		printf("File cannot be accessed!\n");
		return 2;
	}
}

int isRegFile(char* path) {
	struct stat elmt_stat;
	stat(path, &elmt_stat);	
	return S_ISREG(elmt_stat.st_mode);
}


/*
 *	Undo/Redo functions
 */

// Stack operations for undo/redo

bool stackEmpty(optStack ur_stack) {
	return (ur_stack.last_opt == NULL);
}

void stackPush(optStack* ur_stack, optNode* tmp) {
	tmp->next_opt = ur_stack->last_opt;
	ur_stack->last_opt = tmp;
}

void stackPop(optStack* ur_stack) {
	optNode* tmp;
	
	if ( !stackEmpty(*ur_stack) ) {
		tmp = ur_stack->last_opt;
		ur_stack->last_opt = tmp->next_opt;
		deallocURNode(tmp);
	}
}

optNode* allocURNode(char* passed_path, char passed_opt) {
	optNode* new_opt = (optNode*)malloc(sizeof(optNode));
	if ( new_opt == NULL ) {
		printf("Failed to allocate memory!\n");
		exit(EXIT_FAILURE);
	}
	
	new_opt->path = strdup(passed_path);
	new_opt->opt = passed_opt;

	return new_opt;
}

void deallocURNode(optNode* passed_node) {
	if ( passed_node == NULL ) {
		printf("Node not found! Dafuq?\n");
		exit(EXIT_FAILURE);
	}

	free(passed_node->path);
	free(passed_node);
}

void initURStack(optStack* ur_stack) {
	ur_stack->last_opt = NULL;
}

void delURStack(optStack* ur_stack) {
	optNode* tmp;
	tmp = ur_stack->last_opt;

	while ( tmp != NULL ) {
		ur_stack->last_opt = tmp->next_opt;
		deallocURNode(tmp);
		tmp = ur_stack->last_opt;
	}
}


// Undo operations

void undoOpen() {

}

void undoCreate() {

}

void undoDelete() {

}

void undoPaste() {

}


// Redo operations

void redoOpen() {

}

void redoCreate() {

}

void redoDelete() {

}

void redoPaste() {

}
