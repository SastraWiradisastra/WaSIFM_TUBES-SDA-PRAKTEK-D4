#ifndef URIMPLEMENT_H
#define URIMPLEMENT_H

#include "structure.h"

/*
 *	Undo/Redo functions	
 */

// Insertion to stack
void recordOperation(optStack* stack, char opt, char* path1, char* path2);

// Undo operations
void undoCreate(optStack* undo_stack, optStack* redo_stack, fsTree* fs);
void undoDelete(optStack* undo_stack, optStack* redo_stack, fsTree* fs);
void undoPaste(optStack* undo_stack, optStack* redo_stack, fsTree* fs);
void undoMove(optStack* undo_stack, optStack* redo_stack, fsTree* fs);
void undoRename(optStack* undo_stack, optStack* redo_stack, fsTree* fs);  

// Redo operations
void redoCreate(optStack* redo_stack, optStack* undo_stack, fsTree* fs);
void redoDelete(optStack* redo_stack, optStack* undo_stack, fsTree* fs);
void redoPaste(optStack* redo_stack, optStack* undo_stack, fsTree* fs);
void redoMove(optStack* redo_stack, optStack* undo_stack, fsTree* fs);
void redoRename(optStack* redo_stack, optStack* undo_stack, fsTree* fs);  

#endif
