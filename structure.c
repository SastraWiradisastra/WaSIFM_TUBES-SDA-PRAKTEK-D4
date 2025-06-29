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

void initFileSystem(fsTree* fs, char* init_path) {
	fsNode* entry = createFSNode();
	entry->name = strdup(init_path); // Still can't get root to work because of /usr
	fs->root = entry;
	createFSTree(fs->root);
}

void deallocFileSystem(fsNode* node) {
	if ( node == NULL )
		return;

	int tmp_count = countDirElmt(node->name);
	if ( tmp_count < 0 ) 
		return;

	if ( node->children != NULL ) {
		for (int i = 0; i < tmp_count; i++) {
			if ( node->children[i] != NULL ) {
			    deallocFileSystem(node->children[i]);
			}
		}
		free(node->children);
	}
	if ( node->name != NULL ) 
		free(node->name);
	free(node);
}

void createFSTree(fsNode* node) {
	int tmp_count = countDirElmt(node->name);
	if ( tmp_count < 0 ) 
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
	}
	free(tmp_path);
}

// Main file system operations

int countDirElmt(char* path) {
	int len = 0;
	DIR* dir;
	dir = opendir(path);
	if ( dir == NULL )
		return 0; // Somehow works weirdly enough

	struct dirent* entry;
	while ( (entry = readdir(dir)) != NULL ) {
		// Skips current directory, previous directory, and dot files
		if ( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && entry->d_name[0] != '.')
			len++;
	}
	closedir(dir);

	return len;
}

void getDirElmt(fsNode* node) {
	int tmp_count = countDirElmt(node->name);
	if ( tmp_count <= 0 ) {
		node->children = NULL;
		return;
	} // Found why countDirElmt work :p (lupa pernah bikin jir lmao)

	// Double pointer init (dynamic array of pointer to struct fsNode)
	node->children = (fsNode**)malloc(sizeof(fsNode*)*tmp_count);
	if ( node->children == NULL ) {
		printf("Failed to allocate memory!\n");
		exit(EXIT_FAILURE);
	}

	DIR* dir = opendir(node->name);
	if ( dir == NULL ) {
		free(node->children);
		node->children = NULL;
		return;
	}

	struct dirent* entry;
	int i = 0;
	// Use while loop for extra safety with the tmp_count break if statement
	while ( (entry = readdir(dir)) != NULL ) {
		if ( strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_name[0] == '.' ) 
			continue;
		
		if ( i >= tmp_count ) 
			break;

		size_t path_len = strlen(node->name) + strlen(entry->d_name) + 2;
		char* tmp_path = malloc(path_len);
		// If root, then ignore the '/' in node->name (Might not use this one though)
		if ( strcmp(node->name, "/") == 0 ) 
			snprintf(tmp_path, path_len, "/%s", entry->d_name);
		// If not, combine with d_name seperated with '/'
		else 
			snprintf(tmp_path, path_len, "%s/%s", node->name, entry->d_name);

		fsNode* pBuf = createFSNode();
		pBuf->name = tmp_path;  

		node->children[i] = pBuf;
		i++;
	}
	closedir(dir);
}

fsNode* findElmt(char* path, fsNode* node) {
	if ( node == NULL ) {
		printf("Dat thang aint here g!\n");
		return NULL;
	}

	if ( strcmp(node->name, path) == 0 )
		return node; // Return searched node address

	int tmp_count = countDirElmt(node->name);
	for (int i = 0; i < tmp_count; i++) {
		if ( node->children[i] != NULL && isRegFile(node->children[i]->name) == 0 ) {
			// In retrospect, path probably doesn't belong here ngl
			fsNode* tmp_node = findElmt(path, node->children[i]);
			if ( tmp_node != NULL )
				return tmp_node;
		}
	}

	return NULL;
}

void updateDirElmt(char* path, fsNode* entryNode) {
    if (!path || !entryNode)
        return;

    fsNode* target = findElmt(path, entryNode);
    if (!target)
        return;

    // Only deallocate children if they exist
    if (target->children) {
        int count = countDirElmt(target->name);
        for (int i = 0; i < count; i++) {
            if (target->children[i]) {
                // Only dealloc if not currently in use
                if (target->children[i] != entryNode) {
                    deallocFileSystem(target->children[i]);
                }
            }
        }
        free(target->children);
        target->children = NULL;
    }

    getDirElmt(target); // Rebuild children

    // Rebuild tree for new children
    int new_count = countDirElmt(target->name);
    for (int i = 0; i < new_count; i++) {
        if (target->children[i] && !isRegFile(target->children[i]->name)) {
            createFSTree(target->children[i]);
        }
    }
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

optNode* stackPop(optStack* ur_stack) {
    if (stackEmpty(*ur_stack)) 
        return NULL;
    
    optNode* tmp = ur_stack->last_opt;
    ur_stack->last_opt = tmp->next_opt;
    tmp->next_opt = NULL;
    return tmp;
}

optNode* allocURNode(char* path1, char* path2, char passed_opt) {
    optNode* new_opt = (optNode*)malloc(sizeof(optNode));
    if (new_opt == NULL) {
        printf("Failed to allocate memory!\n");
        exit(EXIT_FAILURE);
    }
    
    new_opt->path1 = path1 ? strdup(path1) : NULL;
    new_opt->path2 = path2 ? strdup(path2) : NULL;
    new_opt->opt = passed_opt;
    new_opt->next_opt = NULL;

    return new_opt;
}

void deallocURNode(optNode* passed_node) {
    if (passed_node == NULL) return;

    if (passed_node->path1) free(passed_node->path1);
    if (passed_node->path2) free(passed_node->path2);
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


