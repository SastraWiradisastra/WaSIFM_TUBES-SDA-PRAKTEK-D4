#include "helper.h"

/*
 *	Helper functions for implementations
 */

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
	return S_ISREG(elmt_stat.st_mode); // Returns a non-zero value if regular file
}

char* getParentDirectory(char* path) {
	if (path == NULL) 
		return NULL;

	char* parent = strdup(path);
	if (parent == NULL) 
		return NULL;

	char* last_slash = strrchr(parent, '/');
	if (last_slash != NULL) {
		if (last_slash == parent) 
			parent[1] = '\0'; // If root dir (Potentially unused)
		else 
			*last_slash = '\0';
	}

	return parent;
}

char* getFilename(char* path) {
	if (path == NULL) 
		return NULL;

	char* last_slash = strrchr(path, '/');
	if (last_slash != NULL)
		return last_slash + 1;

	return path;
}

void addToSelection(char* path, char*** selected_files, int* selected_count) {
	if (path == NULL || selected_files == NULL || selected_count == NULL) 
		return;

	*selected_files = realloc(*selected_files, (*selected_count + 1) * sizeof(char*));
	if (*selected_files == NULL) {
		printf("Error: Memory allocation failed\n");
		*selected_count = 0;
		return;
	}

	(*selected_files)[*selected_count] = strdup(path);
	if ((*selected_files)[*selected_count] != NULL)
		(*selected_count)++;
}

void clearSelection(char*** selected_files, int* selected_count) {
	if (selected_files == NULL || selected_count == NULL) return;

	for (int i = 0; i < *selected_count; i++) {
		if ((*selected_files)[i] != NULL)
			free((*selected_files)[i]);
	}
	
	if (*selected_files != NULL) {
		free(*selected_files);
		*selected_files = NULL;
	}
	*selected_count = 0;
}

bool isSelected(char* path, char** selected_files, int selected_count) {
	if (path == NULL || selected_files == NULL) 
		return false;

	for (int i = 0; i < selected_count; i++) {
		if (selected_files[i] != NULL && strcmp(selected_files[i], path) == 0)
			return true;
	}

	return false;
}
