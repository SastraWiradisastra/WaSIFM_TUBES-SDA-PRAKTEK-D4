#ifndef STACK_H
#define STACK_H

#include "helper.h"

// Struct definitions

typedef struct fsNode fsNode;
typedef struct fsTree fsTree;
typedef struct optNode optNode;
typedef struct optStack optStack;

/*
 *	Tree implementation
 *	for file system hierarchy representation
 */

struct fsNode{
	char* name;
	fsNode** children;
};

struct fsTree{
	fsNode* root;
};


/*
 *	Stack implementation
 *	for undo and redo operations
 */

struct optNode{
	char* path1;	// Primary path (source)
	char* path2;	// Secondary path (destination)
	char opt;
	optNode* next_opt;
};

struct optStack{
	optNode* last_opt;
};


/*
 *	File system functions	
 */

// Tree operations
fsNode* createFSNode(void);
void initFileSystem(fsTree* fs, char* init_path);
void deallocFileSystem(fsNode* fs);
void createFSTree(fsNode* node);

// Main file system operations
int countDirElmt(char* path);	
void getDirElmt(fsNode* node);
fsNode* findElmt(char* path, fsNode* node);
void updateDirElmt(char* path, fsNode* entryNode);


/*
 *	Undo/Redo stack functions
 */

bool stackEmpty(optStack ur_stack);
void stackPush(optStack* ur_stack, optNode* tmp);
optNode* stackPop(optStack* ur_stack);
optNode* allocURNode(char* path1, char* path2, char passed_opt);
void deallocURNode(optNode* passed_node);
void initURStack(optStack* ur_stack);
void delURStack(optStack* ur_stack);

#endif
