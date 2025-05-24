#include "structure.h"
#include <pwd.h>

// Struct for getting home directory
struct passwd* home_dir;


/*
 *	Initialization functions
 */

void init(void) {
	uid_t uid = getuid()		// Getting user id 
	home_dir = getpwuid(uid)	// Getting home directory from user id

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
	// Main implement
	return 0;
}
