#include "urimplement.h"
#include "fsimplement.h"

// Helper function for keeping track of undo/redo operations
void recordOperation(optStack* stack, char opt, char* path1, char* path2) {
    optNode* node = allocURNode(path1, path2, opt);
    stackPush(stack, node);
}

// Undo operations
void undoCreate(optStack* undo_stack, optStack* redo_stack, fsTree* fs) {
    if (stackEmpty(*undo_stack)) return;
    
    optNode* op = stackPop(undo_stack);
    if (!op) return;
    
    deleteFile(op->path1, op->path2, fs, NULL);
    stackPush(redo_stack, op);
}

void undoDelete(optStack* undo_stack, optStack* redo_stack, fsTree* fs) {
    if (stackEmpty(*undo_stack)) return;
    
    optNode* op = stackPop(undo_stack);
    if (!op) return;
    
    char* parent = getParentDir(op->path1);
    if (parent) {
        pasteFile(parent, op->path2, fs, NULL);
        free(parent);
    }
    stackPush(redo_stack, op);
}

void undoPaste(optStack* undo_stack, optStack* redo_stack, fsTree* fs) {
    if (stackEmpty(*undo_stack)) return;
    
    optNode* op = stackPop(undo_stack);
    if (!op) return;
    
    char* dest = op->path1;
    DIR* dir = opendir(dest);
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) 
                continue;
            
            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "%s/%s", dest, entry->d_name);
            deleteFile(path, op->path2, fs, NULL);
        }
        closedir(dir);
    }
    stackPush(redo_stack, op);
}

void undoMove(optStack* undo_stack, optStack* redo_stack, fsTree* fs) {
    if (stackEmpty(*undo_stack)) return;
    
    optNode* op = stackPop(undo_stack);
    if (!op) return;
    
    moveFile(op->path2, op->path1, fs, NULL);
    stackPush(redo_stack, op);
}

void undoRename(optStack* undo_stack, optStack* redo_stack, fsTree* fs) {
    if (stackEmpty(*undo_stack)) return;

    optNode* op = stackPop(undo_stack);
    if (!op) return;

    char* old_name = getFilename(op->path1);
    if (old_name) {
        renameFile(op->path2, old_name, fs, NULL);
    }
    stackPush(redo_stack, op);
}

// Redo operations
void redoCreate(optStack* redo_stack, optStack* undo_stack, fsTree* fs) {
    if (stackEmpty(*redo_stack)) return;
    
    optNode* op = stackPop(redo_stack);
    if (!op) return;
    
    createFile(op->path1, fs, NULL);
    stackPush(undo_stack, op);
}

void redoDelete(optStack* redo_stack, optStack* undo_stack, fsTree* fs) {
    if (stackEmpty(*redo_stack)) return;
    
    optNode* op = stackPop(redo_stack);
    if (!op) return;
    
    deleteFile(op->path1, op->path2, fs, NULL);
    stackPush(undo_stack, op);
}

void redoPaste(optStack* redo_stack, optStack* undo_stack, fsTree* fs) {
    if (stackEmpty(*redo_stack)) return;
    
    optNode* op = stackPop(redo_stack);
    if (!op) return;
    
    pasteFile(op->path1, op->path2, fs, NULL);
    stackPush(undo_stack, op);
}

void redoMove(optStack* redo_stack, optStack* undo_stack, fsTree* fs) {
    if (stackEmpty(*redo_stack)) return;
    
    optNode* op = stackPop(redo_stack);
    if (!op) return;
    
    moveFile(op->path1, op->path2, fs, NULL);
    stackPush(undo_stack, op);
}

void redoRename(optStack* redo_stack, optStack* undo_stack, fsTree* fs) {
    if (stackEmpty(*redo_stack)) return;

    optNode* op = stackPop(redo_stack);
    if (!op) return;

    char* new_name = getFilename(op->path2);
    if (new_name) {
        renameFile(op->path1, new_name, fs, NULL);
    }
    stackPush(undo_stack, op);
}
