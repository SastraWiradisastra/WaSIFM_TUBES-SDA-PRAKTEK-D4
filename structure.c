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

void initFileSystem(fsTree fs) {
	fsNode* entry = createFSNode();
	entry->name = '/';
	fs->root = entry;
	createFSTree(fs->root);
}

void deallocFileSystem(fsTree fs) {

}

void createFSTree(fsNode* node) {
	char* tmp_path = strdup(node->name);
	getDirElmt(tmp_path, node);
	for (int i = 0; i < countDirElmt(tmp_path); i++)
		createFSTree(path, node->children[i]);
}

// Main file system operations

int countDirElmt(char* path) {
	int len = 0;
	DIR* dir;
	dir = opendir(path);
	if ( dir == NULL )
		return -1;

	struct dirent* entry;
	while ( (entry = readdir(dir)) != NULL ) {
		if ( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
			len++;
	}
	closedir(dir);

	return len;
}

void getDirElmt(char* path, fsNode* node) {
	node->*children = (fsNode*)malloc(sizeof(fsNode)*countDirElmt(path));
	if ( *children == NULL ) {
		printf("Failed to allocate memory!\n");
		exit(EXIT_FAILURE);
	}

	DIR* dir;
	dir = opendir(path);
	if ( dir == NULL )
		return;

	struct dirent* entry;
	for (int i = 0; (entry = readdir(dir)) != NULL && i < countDirElmt(path); i++) {
		if ( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
			fsNode* pBuf = createFSNode();
			pBuf->name = strdup(entry->d_name);
			node->children[i] = pBuf;
		}
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
	return (ur_stack->last_opt == NULL);
}

void stackPush(optStack ur_stack, optNode* tmp) {
	tmp->next_opt = ur_stack->last_opt;
	ur_stack->last_opt = tmp;
}

void stackPop(optStack ur_stack) {
	optNode* tmp;
	
	if ( !stackEmpty(ur_stack) ) {
		tmp = ur_stack->last_opt;
		ur_stack->last_opt = tmp->next_opt;
		deallocURNode(tmp);
	}
}

optNode* allocURNode(char* passed_path, char passed_opt) {
	optNode* new_opt = (optNode*)malloc(optNode);
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

	free(optNode->path);
	free(optNode);
}

void initURStack(optStack ur_stack) {
	ur_stack->root = NULL;
}

void delURStack(optStack ur_stack) {
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
