#include "structure.h"

/*
 *	Core file system operations
 */

void openFile(char* path) {

}

void selectFile() {

}

void searchFile() {

}

void createFile(char* path) {

}

void deleteFile(char* path) {

}

void copyFile(char* path, char* dest_path) {
	FILE* f = fopen(path, "r");
	if (f == NULL) {
		printf("Path does not exist!\n");
		return;
	}
}

void moveFile(char* path, char* dest_path) {
	FILE* f = fopen(path, "r");
	if (f == NULL) {
		printf("Path does not exist!\n");
		return;
	}
}

void renameFile(char* path, char* new_name) {
	FILE* f = fopen(path, "r");
	if (f == NULL) {
		printf("Path does not exist!\n");
		return;
	}
}


/*
 *	Undo/Redo functions
 */

void undoOpen() {

} // For opening directories

void undoCreate() {

}

void undoDelete() {

}

void undoPaste() {

}

void undoMove() {

}


// Redo operations

void redoOpen() {

} // For opening directories

void redoCreate() {

}

void redoDelete() {

}

void redoPaste() {

}

void redoMove() {

}
