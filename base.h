#ifndef BASE_H
#define BASE_H

// Base directories used in all files

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>


/* 
 *	Program Keybinds
 *	(As per progress report #3)
 */

// Navigation

#define KEY_NAVUP	'j' 
#define KEY_NAVDOWN	'k' 
#define KEY_NAVRIGHT	'l' 
#define KEY_NAVLEFT	'h' 
#define KEY_PARENT	'\b' 
#define KEY_EXIT	'q'


// File Manipulation

#define KEY_SELECT	' '
#define KEY_OPEN	'\r'	// Fairly sure carriage return is [ENTER]
#define KEY_CREATE	'NULL'	// Not yet set
#define KEY_DELETE	'd'	// For delete/cut using the same key
#define KEY_RENAME	'a'
#define KEY_COPY	'y'
#define KEY_PASTE	'p'


// Program state

#define KEY_UNDO	'u'
#define KEY_REDO	'NULL'	// Not yet set


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
 *	(Possibly not used)
 */

#endif
