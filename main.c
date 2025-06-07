#include "structure.h"
#include <pwd.h>

// Struct for getting home directory
struct passwd* home_dir;


/*
 *	Initialization functions
 */

void init(void) {
	uid_t uid = getuid();		// Getting user id 
	home_dir = getpwuid(uid);	// Getting home directory from user id

	/* Main logic init goes here */
	 
	// Set the shell
	if( getenv("SHELL") == NULL )
	{
		shell = malloc(10);
		if( shell == NULL ) {
			printf("%s\n", "Couldn't initialize shell");
			exit(1);
		}
		snprintf(shell, 10, "%s", "/bin/bash"); // Set bash as default if not set
	}
	else
	{
		allocSize = snprintf(NULL, 0, "%s", getenv("SHELL"));
		shell = malloc(allocSize + 1);
		if( shell == NULL ) {
			printf("%s\n", "Couldn't initialize shell");
			exit(1);
		}
		snprintf(shell, allocSize+1, "%s", getenv("SHELL"));
	}
}

void curses_init(void) {
	// UI init goes here
}


/*
 *	WOOHOO MAIN IMPLEMENT!!1!
 *	(Belum jadi udah ada jir)
 */

int main(void) {
	// Init function calls
	init();
	cursesinit(); 

	// Current path is stored here initialized with /home/[USER]
	char* curr_path = strdup(home_dir->pw_dir);
	char* temp_path; // Temp directory path for file system manipulation
	char** curr_elmts; // For directory child element display
	char** sel_files;
	int sel_count;

	// Potentially moved elsewhere
	char* clp_path; 
	char* trash_path;

	// Undo redo stack initialization
	optStack undo_stack;
	optStack redo_stack;

	// File system tree initialization and creation
	fsTree file_system;
	initFileSystem(&file_system, curr_path);

	do

	/* Fun stuff here */
	
	while(input != 'q');

	deallocFileSystem(file_system.root);
	
	return 0;
}
