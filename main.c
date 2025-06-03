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

	// Main logic init goes here
	 
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
	// cursesinit(); << Not used currently

	// Current path is stored here initialized with /home/[USER]
	char* curr_path = strdup(home_dir->pw_dir);
	
	// file system tree initialization and creation
	fsTree file_system;
	initFileSystem(&file_system, curr_path);

	do

	/* Fun stuff here */
	
	while(input != 'q');

	deallocFileSystem(file_system.root);
	
	return 0;
}
