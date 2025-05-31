#ifndef STACK_H
#define STACK_H

#include "base.h"

/*
 *	Tree implementation
 *	for file system hierarchy representation
 */

typedef struct fsNode {
	char* name;
	fsNode** children;
} fsNode;

typedef struct fsTree {
	fsNode* root;
} tree;


/*
 *	Stack implementation
 *	for undo and redo operations
 */

typedef struct optNode {
	char* path;
	char opt;
	optNode* next_opt;
}

typedef struct optStack {
	optNode* last_opt;
}


/*
 *	File system functions	
 */

// Tree operations
fsNode* createFSNode(void);
void initFileSystem(fsTree fs);
void deallocFileSystem(fsTree fs);
void createFSTree(fsNode* node);

// Main file system operations
int countDirElmt(char* path);	
void getDirElmt(char* path, fsNode* node);
int pathExists(char* path);
int dirPermissions(char* path);
int isRegFile(char* path);


/*
 *	Undo/Redo functions
 */

// Stack operations for undo/redo
bool stackEmpty(optStack ur_stack);
void stackPush(optStack ur_stack, optNode* tmp);
void stackPop(optStack ur_stack);
void allocURNode(char* passed_path, char passed_opt);
void deallocURNode(optNode* passed_node);
void initURStack(optStack ur_stack);
void delURStack(optStack ur_stack);


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
