#ifndef FSIMPLEMENT_H
#define FSIMPLEMENT_H

#include "helper.h"

/*
 *	Core file system operations
 */

// No state change
void openFile(char* path);
void selectFile(char* path, char*** selected_files, int* selected_count);
void searchFile();

// Change in state
void createFile(char* path, fsTree* fs, optStack* undo_stack, optStack* redo_stack);
void deleteFile(char* path, fsTree* fs, optStack* undo_stack, optStack* redo_stack);
void copyFile(char* path, char* dest_path, fsTree* fs, optStack* undo_stack, optStack* redo_stack);
void moveFile(char* path, char* dest_path, fsTree* fs, optStack* undo_stack, optStack* redo_stack);
void renameFile(char* path, char* new_name, fsTree* fs, optStack* undo_stack, optStack* redo_stack);
void cutFile(char* path, char** clipboard_path, bool* clipboard_is_cut);
void pasteFile(char* dest_dir, char* clipboard_path, bool* clipboard_is_cut, fsTree* fs, optStack* undo_stack, optStack* redo_stack);

#endif
