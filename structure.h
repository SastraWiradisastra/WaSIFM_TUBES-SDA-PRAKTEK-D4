#ifndef STACK_H
#define STACK_H

#include "base.h"

/*
 *	Tree + Stack implementation
 *	for file system hierarchy representation
 */

typedef struct treeNode {
	char* name;
	
	// Possibly irrelevant because of S_IFREG and S_IFDIR
	char type; 		// Either file or directory
	
	char* last_modified;
	treeNode* next_element;	
} treeNode;

typedef struct tree {
	treeNode* root;
} tree;

// Stack implementation for keeping tab of current paths 
typedef struct dirStack {
	treeNode* curr_dir;
} dirStack;


/*
 *	Stack implementation
 *	for undo and redo operations
 */

typedef struct optNode {
	char* path;
	char opt;
}

typedef struct optStack {
	optNode* last_opt;
}


/*
 *	Program specific structures
 */

typedef struct fileType {
	treeNode file;
	long size_byte;
	char* format;
} fileType;

typedef struct dirType {
	treeNode directory;
	treeNode* first_son;
} dirType;


// Stack NULL checking
bool stackEmpty();


/*
 *	File system functions	
 */


/*
 *	Undo/Redo functions
 */

// Undo operations

void undoOpen();
void undoCreate();
void undoDelete();
void undoPaste();


// Redo operations

void redoOpen();
void redoCreate();
void redoDelete();
void redoPaste();

#endif
