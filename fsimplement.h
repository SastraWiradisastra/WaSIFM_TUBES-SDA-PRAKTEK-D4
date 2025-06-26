#ifndef FSIMPLEMENT_H
#define FSIMPLEMENT_H

#include "structure.h"

/*
 *	Core file system operations
 */

// No state change
void openFile(char* path);
void searchFile(char* curr_path, char* clipboard_dir, fsTree* fs, int* selected_index);

// Change in state
void createFile(char* path, fsTree* fs, optStack* undo_stack);
void deleteFile(char* path, char* clp_path, fsTree* fs, optStack* undo_stack);
void copyFile(char* path, char* clp_path);
void moveFile(char* path, char* dest_path, fsTree* fs, optStack* undo_stack);
void renameFile(char* path, char* new_name, fsTree* fs, optStack* undo_stack);
void clearClipboard(char* clipboard_dir);
void pasteFile(char* dest_dir, char* clp_path, fsTree* fs, optStack* undo_stack);

#endif
