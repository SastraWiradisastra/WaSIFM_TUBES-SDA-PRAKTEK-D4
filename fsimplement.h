#ifndef FSIMPLEMENT_H
#define FSIMPLEMENT_H

#include "helper.h"

/*
 *	Core file system operations
 */

// No state change
void openFile(char* path, char* curr_path);
void selectFile(char* path, char*** selected_files, int* selected_count);
void searchFile(void);

// Change in state
void createFile(char* path, fsTree* fs);
void deleteFile(char* path, char* clp_path, fsTree* fs);
void copyFile(char* path, char* clp_path, fsTree* fs);
void moveFile(char* path, char* dest_path, fsTree* fs);
void renameFile(char* path, char* new_name, fsTree* fs);
void pasteFile(char* dest_dir, char* clp_path, fsTree* fs);

// Trash operations 
void clearTrash(void);
void restoreTrash(char* name);

#endif
