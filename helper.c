#include "helper.h"

/*
 *	Helper functions for implementations
 */

int pathExists(char* path) {
	return access(path, F_OK) != 0;
} 

int dirPermissions(char* path) {
	if (access(path, R_OK) == 0 && access(path, W_OK) == 0)
		return 0; // Readable and writeable
	else if (access(path, R_OK) == 0)
		return 1; // Readable only
	else	
		return 2; // No permissions
}

int isRegFile(char* path) {
	struct stat elmt_stat;
	stat(path, &elmt_stat);	
	return S_ISREG(elmt_stat.st_mode); // Returns a non-zero value if regular file
}

char* getParentDir(char* path) {
	if (path == NULL) 
		return NULL;

	char* parent = strdup(path);
	if (parent == NULL) 
		return NULL;

	char* token = strrchr(parent, '/');
	if (token != NULL) {
		if (token == parent) 
			parent[1] = '\0'; // If root dir (Potentially unused)
		else 
			*token = '\0';
	}

	return parent;
}

char* getFilename(char* path) {
	if (path == NULL) 
		return NULL;

	char* token = strrchr(path, '/'); // Get last occurance of '/'
	if (token != NULL)
		return token + 1;

	return path;
}

char* formatSize(long bytes) {
	char* size_str = malloc(20);
	const char* units[] = {"B", "KB", "MB", "GB", "TB"};
	int unit = 0;
	double size = bytes;

	while (size >= 1024 && unit < 4) {
		size /= 1024;
		unit++;
	}

	if (unit == 0)
		snprintf(size_str, 20, "%ld %s", bytes, units[unit]);
	else
		snprintf(size_str, 20, "%.2f %s", size, units[unit]);
	
	return size_str;
}
