#ifndef URIMPLEMENT_H
#define URIMPLEMENT_H

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
