#ifndef IMPLEMENT_H
#define IMPLEMENT_H

/*
 *	Core file system operations
 */

// No state change
void openFile(char* path);
void selectFile();
void searchFile();

// Change in state
void createFile(char* path);
void deleteFile(char* path);
void copyFile(char* path, char* dest_path);
void moveFile(char* path, char* dest_path);
void renameFile(char* path, char* new_name);


/*
 *	Undo/Redo functions	
 */

// Undo operations
void undoOpen();
void undoCreate();
void undoDelete();
void undoPaste();
void undoMove();

// Redo operations
void redoOpen();
void redoCreate();
void redoDelete();
void redoPaste();
void redoMove();

#endif
