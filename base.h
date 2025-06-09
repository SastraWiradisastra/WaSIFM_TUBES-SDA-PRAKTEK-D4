#ifndef BASE_H
#define BASE_H

// All libraries used in the program

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <curses.h>
#include <time.h>


/* 
 *	Program Keybinds (Hopefully finalized)
 */

// Navigation

#define KEY_NAVUP	'j' 
#define KEY_NAVDOWN	'k' 
#define KEY_NAVRIGHT'l' 
#define KEY_NAVLEFT	'h' 
#define KEY_PARENT	'KEY_BACKSPACE' 
#define KEY_EXIT	'q'


// File Manipulation

#define KEY_SELECT	' '
#define KEY_OPEN	'KEY_ENTER'	
#define KEY_CREATE	'n'	
#define KEY_DELETE	'd'	// For delete/cut using the same key
#define KEY_RENAME	'a'
#define KEY_MOVE	'm' 
#define KEY_COPY	'y'
#define KEY_PASTE	'p'


// Program state

#define KEY_UNDO	'u'
#define KEY_REDO	'r'	


// Searching

#define KEY_SEARCH	'f'
#define KEY_FILTER	'.'
#define KEY_SORT	'o'


/*
 *	UI Elements
 *	(Not yet made)
 */


/*
 *	System Paths
 *	(Possibly not used, replaced by global vars in main)
 */


#endif
