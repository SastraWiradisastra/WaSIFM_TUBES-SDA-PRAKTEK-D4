#ifndef HELPER_H
#define HELPER_H

#include "base.h"

/*
 *	Helper functions for implementations
 */

// Verification helper functions
int pathExists(char* path);
int dirPermissions(char* path);
int isRegFile(char* path);

// Getter functions
char* getFilename(char* path);
char* getParentDir(char* path);

// Misc. functions
char* formatSize(long bytes);

#endif
