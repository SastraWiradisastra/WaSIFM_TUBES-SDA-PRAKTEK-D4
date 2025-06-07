#ifndef HELPER_H
#define HELPER_H

#include "structure.h"

/*
 *	Helper functions for implementations
 */

// General helper functions
int pathExists(char* path);
int dirPermissions(char* path);
int isRegFile(char* path);

// Getter functions
char* getFilename(char* path);
char* getParentDir(char* path);

// Selection helper functions
void addToSelection(char* path, char*** selected_files, int* selected_count);
void clearSelection(char*** selected_files, int* selected_count);
bool isSelected(char* path, char** selected_files, int selected_count);

#endif
