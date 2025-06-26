#include "base.h"
#include "fsimplement.h"
#include "urimplement.h"
#include "helper.h"
#include "UI.h"
#include <pwd.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>

// Global variables
struct passwd* home_dir;
bool running = true;
int color_offset = 0;
clock_t last_anim_time = 0;
UIWindows ui;
UIState state = {0};
char* curr_path = NULL;
char** dir_items = NULL;
char** sorted_dir = NULL;
int dir_count = 0;
char** tree_items = NULL;
int tree_count = 0;
char** tree_full_paths = NULL;
volatile sig_atomic_t resize_flag = 0; // Resizing signal handling to avoid segfaults
char status_msg[256] = {0}; // For status printing
time_t status_time = 0; // Status display duration

// Main helper functions 
void refreshDirView(char*** dir_items, int* dir_count, char* path);
void refreshTreeView(fsNode* node, int depth, int* lastFlags, char*** tree_items, int* tree_count, char*** tree_full_paths);
void truncateString(WINDOW* win, char* str, int max_width, int y, int x);
void updateAnimation(void);
int findTreeIndex(char** tree_full_paths, int tree_count, char* path);
void refreshTreeSel(UIWindows* ui, UIState* state, char** tree_full_paths, int tree_count, char* curr_path);

/*
 *	Status printing
 */
void setStatus(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(status_msg, sizeof(status_msg), fmt, args);
    va_end(args);
    status_time = time(NULL);
}

/*
 *  Handle window resize  
 */
void handleResize(int sig) {
    if (sig == SIGWINCH) 
        resize_flag = 1;
}

/*
 *	Refresh directory contents
 */
void refreshDirView(char*** dir_items, int* dir_count, char* path) {
    // Free existing directory items
    if (*dir_items) {
        for(int i = 0; i < *dir_count; i++) {
            if ((*dir_items)[i]) 
	    	free((*dir_items)[i]);
        }
        free(*dir_items);
	*dir_items = NULL;
    }
	
    *dir_count = countDirElmt(path);
    if (*dir_count <= 0) {
        *dir_items = NULL;
        return;
    }

    *dir_items = malloc(*dir_count * sizeof(char*));
    DIR* dir = opendir(path);
    if (!dir) return;

    struct dirent* entry;
    int i = 0;
    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && 
           strcmp(entry->d_name, "..") != 0 && 
           entry->d_name[0] != '.') {
            (*dir_items)[i] = strdup(entry->d_name);
            i++;
        }
    }
    closedir(dir);
}

/*
 *  Refresh tree view with proper hierarchy
 */
void refreshTreeView(fsNode* node, int depth, int* lastFlags, char*** tree_items, int* tree_count, char*** tree_full_paths) {
    if (!node) 
        return;

    // Free existing tree data only at the root level
    if (depth == 0 && *tree_items) {
        for(int i = 0; i < *tree_count; i++) {
            free((*tree_items)[i]);
            free((*tree_full_paths)[i]);
        }
        free(*tree_items);
        free(*tree_full_paths);
        *tree_items = NULL;
        *tree_full_paths = NULL;
        *tree_count = 0;
    }

    // Allocate display string
    char* display = malloc(256);
    memset(display, 0, 256);
    
    // Add hierarchy lines
    for (int i = 0; i < depth; i++) {
        if (i == depth - 1) {
            strcat(display, "+-> ");
        } else {
            strcat(display, lastFlags[i] ? "    " : "|   ");
        }
    }
    
    // Add node name
    char* name = getFilename(node->name);
    strcat(display, name);
    
    // Add to tree items
    *tree_items = realloc(*tree_items, (*tree_count + 1) * sizeof(char*));
    *tree_full_paths = realloc(*tree_full_paths, (*tree_count + 1) * sizeof(char*));
    
    (*tree_items)[*tree_count] = strdup(display);
    (*tree_full_paths)[*tree_count] = strdup(node->name);
    free(display);
    (*tree_count)++;

    // Process children
    int childCount = countDirElmt(node->name);
    if (childCount > 0 && node->children) {
        int* newLastFlags = malloc((depth + 1) * sizeof(int));
        memcpy(newLastFlags, lastFlags, depth * sizeof(int));
        
        for (int i = 0; i < childCount; i++) {
            if (node->children[i]) {
                newLastFlags[depth] = (i == childCount - 1);
                refreshTreeView(node->children[i], depth + 1, newLastFlags, 
                               tree_items, tree_count, tree_full_paths);
            }
        }
        free(newLastFlags);
    }
}

/*
 *  Find tree index for current path
 */
int findTreeIndex(char** tree_full_paths, int tree_count, char* path) {
    for (int i = 0; i < tree_count; i++) {
        if (strcmp(tree_full_paths[i], path) == 0) {
            return i;
        }
    }
    return -1;
}

/*
 *  Update tree selection to match current directory
 */
void refreshTreeSel(UIWindows* ui, UIState* state, char** tree_full_paths, int tree_count, char* curr_path) {
    int index = findTreeIndex(tree_full_paths, tree_count, curr_path);
    if (index >= 0) {
        state->highlight2 = index;
	state->tree_top = index;
        
        // Ensure selected item is visible
        int tree_win_height = getmaxy(ui->winTree) - 4;
        if (state->highlight2 < state->tree_top) {
            state->tree_top = state->highlight2;
        } else if (state->highlight2 >= state->tree_top + tree_win_height) {
            state->tree_top = state->highlight2 - tree_win_height + 1;
        }
    }
}

/*
 *	Truncate string for display
 */
void truncateString(WINDOW* win, char* str, int max_width, int y, int x) {
    int width = getmaxx(win) - x - 2;  // Leave space for borders
    if (width > max_width) width = max_width;
    
    if ((int)strlen(str) > width) {
        char buf[width + 1];
        strncpy(buf, str, width - 3);
        buf[width - 3] = '\0';
        strcat(buf, "...");
        mvwprintw(win, y, x, "%s", buf);
    } else {
        mvwprintw(win, y, x, "%s", str);
    }
}

/*
 *	Update animation if needed
 */
void updateAnimation(void) {
    clock_t now = clock();
    if((double)(now - last_anim_time)/CLOCKS_PER_SEC >= 0.1) {
        color_offset = (color_offset + 1) % 6;
        last_anim_time = now;
    }
}

/*
 *	Main program
 */
int main(void) {
    // Get home directory of user
    uid_t uid = getuid();
    home_dir = getpwuid(uid);

    initUI();

    curr_path = strdup(home_dir->pw_dir);
    char* clipboard_dir = malloc(strlen(home_dir->pw_dir) + strlen("/.cache/WaSIFM") + 1);
    sprintf(clipboard_dir, "%s/.cache/WaSIFM", home_dir->pw_dir);
    mkdir(clipboard_dir, 0700);

    optStack undo_stack;
    optStack redo_stack;
    initURStack(&undo_stack);
    initURStack(&redo_stack);

    fsTree file_system;
    initFileSystem(&file_system, curr_path);  // Build from XDG_HOME

    // Initialize tree view
    int homeLastFlag = 1;
    refreshTreeView(file_system.root, 0, &homeLastFlag, &tree_items, &tree_count, &tree_full_paths);

    initWindows(&ui);
    refreshDirView(&dir_items, &dir_count, curr_path);

    // Find current path in tree view
    refreshTreeSel(&ui, &state, tree_full_paths, tree_count, curr_path);

    while(running) {
	updateWindowBorders(&ui);

    	// Checker for resize flags for screen changes
        if (resize_flag) {
            resize_flag = 0;
            resize_term(0, 0);
            resizeWindows(&ui);
        }
	
	// Checker for status message for clearing
	if (status_msg[0] != '\0') {
	    // Show status for 3 seconds
	    if (time(NULL) - status_time < 3) {
		move(LINES-1, 0);
		clrtoeol();
		attron(A_BOLD);
		printw("%s", status_msg);
		attroff(A_BOLD);
	    } else {
		// Clear status after timeout
		move(LINES-1, 0);
		clrtoeol();
		status_msg[0] = '\0';
	    }
	} else {
	    // Clear status line when no message
	    move(LINES-1, 0);
	    clrtoeol();
	}

        // Get input without blocking
        nodelay(ui.cur_window, TRUE);
        int ch = wgetch(ui.cur_window);
        nodelay(ui.cur_window, FALSE);
        
        if(ch != ERR) {
            switch(ch) {
                case 'q': 
                    running = false; 
                    break;
                    
                // Window navigation
                case 'h':
                    // Switch to tree window
                    ui.cur_window = ui.winTree;
		    updateWindowBorders(&ui);
                    break;
                    
                case 'l':
                    // Switch to directory window
                    ui.cur_window = ui.winDir;
                    state.highlight = 0;
                    state.dir_top = 0;
		    updateWindowBorders(&ui);
                    break;
                    
                // Item navigation
                case 'j': // Move up
                    if(ui.cur_window == ui.winTree) {
                        if(--state.highlight2 < 0) state.highlight2 = 0;
                        if(state.highlight2 < state.tree_top) state.tree_top = state.highlight2;
                    } else {
                        if(--state.highlight < 0) state.highlight = 0;
                        if(state.highlight < state.dir_top) state.dir_top = state.highlight;
                    }
                    break;
                    
                case 'k': // Move down
                    if(ui.cur_window == ui.winTree) {
                        if(++state.highlight2 >= tree_count) state.highlight2 = tree_count - 1;
                        if(state.highlight2 >= state.tree_top + (getmaxy(ui.winTree) - 4))
                            state.tree_top = state.highlight2 - (getmaxy(ui.winTree) - 5);
                    } else {
                        if(++state.highlight >= dir_count) state.highlight = dir_count - 1;
                        if(state.highlight >= state.dir_top + (getmaxy(ui.winDir) - 4))
                            state.dir_top = state.highlight - (getmaxy(ui.winDir) - 5);
                    }
                    break;
                    
                // Directory navigation
                case KEY_BACKSPACE: // Move to parent directory
                case 127: // Handle backspace key
                case 8:   // Alternative backspace code
                    {
		    	// Prevents going back further than home directory
			if (strcmp(curr_path, home_dir->pw_dir) == 0) {
			    setStatus("Cannot go further than user home directory!");
			    break;
			}

                        char* parent = getParentDir(curr_path);
                        if(parent && strcmp(parent, curr_path) != 0) {
                            free(curr_path);
                            curr_path = strdup(parent);
                            refreshDirView(&dir_items, &dir_count, curr_path);
                            updateDirElmt(curr_path, file_system.root);
                            state.highlight = 0;
                            state.dir_top = 0;
                            
                            // Update tree selection to match new directory
                            refreshTreeSel(&ui, &state, tree_full_paths, tree_count, curr_path);
                        }
                        free(parent);
                    }
                    break;
                    
                // Open/select items
                case '\n': // Enter key
                    if(ui.cur_window == ui.winTree) {
                        if (state.highlight2 >= 0 && state.highlight2 < tree_count) {
                            char* new_path = tree_full_paths[state.highlight2];
                            if (strcmp(new_path, curr_path) != 0 && (isRegFile(new_path) == 0)) {
                                free(curr_path);
                                curr_path = strdup(new_path);
                                refreshDirView(&dir_items, &dir_count, curr_path);
                                state.highlight = 0;
                                state.dir_top = 0;
                            }
                        }
                        ui.cur_window = ui.winDir;
                    } else if(dir_count > 0 && state.highlight < dir_count) {
                        char path[PATH_MAX];

			// Sorted directory handling when entering directories
			if(state.IsSorted && sorted_dir) 
            		    snprintf(path, PATH_MAX, "%s/%s", curr_path, sorted_dir[state.highlight]);
        		else
            		    snprintf(path, PATH_MAX, "%s/%s", curr_path, dir_items[state.highlight]);
                        
			snprintf(path, PATH_MAX, "%s/%s", curr_path, dir_items[state.highlight]);
                        if(!isRegFile(path)) {
                            free(curr_path);
                            curr_path = strdup(path);
                            refreshDirView(&dir_items, &dir_count, curr_path);
                            state.highlight = 0;
                            state.dir_top = 0;
                            
                            // Update tree selection to match new directory
                            refreshTreeSel(&ui, &state, tree_full_paths, tree_count, curr_path);
                        } else {
                            openFile(path);
                        }
                    }
                    break;
                    
                // Sorting toggle
                case 'o':
                    state.IsSorted = !state.IsSorted;
                    if(state.IsSorted) {
                        // Free existing sorted data if any
                        if(sorted_dir) {
                            for(int i = 0; i < dir_count; i++) {
                                if(sorted_dir[i]) free(sorted_dir[i]);
                            }
                            free(sorted_dir);
                        }
                        // Create new sorted array
                        sorted_dir = malloc(dir_count * sizeof(char*));
                        sortDirectory(dir_items, dir_count, sorted_dir);
                    }
                    state.highlight = 0;
                    state.dir_top = 0;
                    break;
 
                // File operations
                case 'n': { // Create new file
                    char new_name[256] = {0};
		    echo();
		    
		    // Use separate line for input
		    move(LINES-2, 0);
		    clrtoeol();
		    printw("Enter file name: ");
		    
		    // Get input 
		    int ch;
		    int idx = 0;
		    while ((ch = getch()) != '\n' && ch != KEY_ENTER && ch != ERR) {
			if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
			    if (idx > 0) new_name[--idx] = '\0';
			}
			else if (isprint(ch) && idx < (int)sizeof(new_name)-1) {
			    new_name[idx++] = ch;
			    new_name[idx] = '\0';
			}
			
			move(LINES-2, 0);
			clrtoeol();
			printw("Enter file name: %s", new_name);
			refresh();
		    }
		    noecho();
		    
		    // Clear input line
		    move(LINES-2, 0);
		    clrtoeol();
		    refresh();
		    
		    // Validate input
		    if (strlen(new_name) == 0) {
			setStatus("File creation cancelled");
			break;
		    }
		    
		    // Check for invalid characters
		    if (strchr(new_name, '/') != NULL) {
			setStatus("Error: Filename cannot contain '/'");
			break;
		    }
		    
		    char new_path[PATH_MAX];
		    snprintf(new_path, PATH_MAX, "%s/%s", curr_path, new_name);
		    
		    if (pathExists(new_path) == 0) {
			setStatus("Error: File already exists");
		    } else {
			createFile(new_path, &file_system, &undo_stack);
			setStatus("Created: %s, in directory: %s", new_name, curr_path);
			refreshDirView(&dir_items, &dir_count, curr_path);
			homeLastFlag = 1;
			refreshTreeView(file_system.root, 0, &homeLastFlag, &tree_items, &tree_count, &tree_full_paths);
			refreshTreeSel(&ui, &state, tree_full_paths, tree_count, curr_path);
		    }
		    break;
		}
                case 'd': // Delete file
                    if(dir_count > 0 && state.highlight < dir_count) {
                        char path[PATH_MAX];
                        snprintf(path, PATH_MAX, "%s/%s", curr_path, dir_items[state.highlight]);
                        deleteFile(path, clipboard_dir, &file_system, &undo_stack);
			setStatus("Deleted: %s, in directory: %s", dir_items[state.highlight], curr_path);
			refreshDirView(&dir_items, &dir_count, curr_path);
			homeLastFlag = 1;
			refreshTreeView(file_system.root, 0, &homeLastFlag, &tree_items, &tree_count, &tree_full_paths);
			refreshTreeSel(&ui, &state, tree_full_paths, tree_count, curr_path);
                    }
                    break;
		case 'a': { // Rename file
		    if(dir_count > 0 && state.highlight < dir_count) {
			char old_path[PATH_MAX];
			snprintf(old_path, PATH_MAX, "%s/%s", curr_path, dir_items[state.highlight]);
			
			char new_name[256] = {0};
			echo();
			
			// Use separate line for input
			move(LINES-2, 0);
			clrtoeol();
			printw("Enter new name: ");
			
			// Get input 
			int ch;
			int idx = 0;
			while ((ch = getch()) != '\n' && ch != KEY_ENTER && ch != ERR) {
			    if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
				if (idx > 0) new_name[--idx] = '\0';
			    }
			    else if (isprint(ch) && idx < (int)sizeof(new_name)-1) {
				new_name[idx++] = ch;
				new_name[idx] = '\0';
			    }
			    
			    move(LINES-2, 0);
			    clrtoeol();
			    printw("Enter new name: %s", new_name);
			    refresh();
			}
			noecho();
			
			// Clear input line
			move(LINES-2, 0);
			clrtoeol();
			refresh();
			
			// Validate input
			if (strlen(new_name) == 0) {
			    setStatus("Rename cancelled");
			    break;
			}
			
			// Check for invalid characters
			if (strchr(new_name, '/') != NULL) {
			    setStatus("Error: Filename cannot contain '/'");
			    break;
			}
			
			char new_path[PATH_MAX];
			snprintf(new_path, PATH_MAX, "%s/%s", curr_path, new_name);
			
			if (pathExists(new_path) == 0) {
			    setStatus("Error: File already exists");
			} else {
			    renameFile(old_path, new_name, &file_system, &undo_stack);
			    setStatus("Renamed: %s to %s, in directory: %s", dir_items[state.highlight], new_name, curr_path);
			    refreshDirView(&dir_items, &dir_count, curr_path);
			    homeLastFlag = 1;
			    refreshTreeView(file_system.root, 0, &homeLastFlag, &tree_items, &tree_count, &tree_full_paths);
			    refreshTreeSel(&ui, &state, tree_full_paths, tree_count, curr_path);
			}
		    }
		    break;
		}
		case 'y': // Copy file
                    if(dir_count > 0 && state.highlight < dir_count) {
                        char path[PATH_MAX];
                        snprintf(path, PATH_MAX, "%s/%s", curr_path, dir_items[state.highlight]);
                        copyFile(path, clipboard_dir);
			setStatus("Copied: %s, in directory: %s", dir_items[state.highlight], curr_path);
                    }
                    break;
                case 'm': { // Move file
                    if(dir_count > 0 && state.highlight < dir_count) {
		        char src_path[PATH_MAX];
		        snprintf(src_path, PATH_MAX, "%s/%s", curr_path, dir_items[state.highlight]);
		
		        char dest_path[PATH_MAX] = {0};
		    	echo();
		
		    	// Use separate line for input
		    	move(LINES-2, 0);
		    	clrtoeol();
		    	printw("Enter destination path: ");
		
		    	// Get input 
		    	int ch;
		    	int idx = 0;
		    	while ((ch = getch()) != '\n' && ch != KEY_ENTER && ch != ERR) {
			    if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
			        if (idx > 0) dest_path[--idx] = '\0';
			    }
			    else if (isprint(ch) && idx < (int)sizeof(dest_path)-1) {
			        dest_path[idx++] = ch;
			        dest_path[idx] = '\0';
			    }   
			
			    move(LINES-2, 0);
			    clrtoeol();
			    printw("Enter destination path: %s", dest_path);
			    refresh();
		        }
		        noecho();
		
		    	// Clear input line
		    	move(LINES-2, 0);
		    	clrtoeol();
		    	refresh();
		
		    	// Validate input
		    	if (strlen(dest_path) == 0) {
		            setStatus("Move cancelled");
		            break;
		        }
		
		    	// Validate destination path
		    	struct stat st;
		    	if (stat(dest_path, &st) != 0) {
		            setStatus("Error: Destination path does not exist");
		            break;
		    	}
		
		    	// Ensure destination is a directory
		    	if (!S_ISDIR(st.st_mode)) {
		            setStatus("Error: Destination must be a directory");
		            break;
		    	}
		
		    	// Check destination permissions
		    	if (dirPermissions(dest_path) == 2) {
		            setStatus("Error: No write permission for destination");
		            break;
		    	}
		
		    	moveFile(src_path, dest_path, &file_system, &undo_stack);
		    	setStatus("Moved: %s, from %s to %s", dir_items[state.highlight], curr_path, dest_path);
		    	refreshDirView(&dir_items, &dir_count, curr_path);
			homeLastFlag = 1;
			refreshTreeView(file_system.root, 0, &homeLastFlag, &tree_items, &tree_count, &tree_full_paths);
			refreshTreeSel(&ui, &state, tree_full_paths, tree_count, curr_path);
	            }
	            break;
		}
                case 'p': // Paste file
                    pasteFile(curr_path, clipboard_dir, &file_system, &undo_stack);
		    setStatus("Files pasted from clipboard");
		    refreshDirView(&dir_items, &dir_count, curr_path);
		    homeLastFlag = 1;
		    refreshTreeView(file_system.root, 0, &homeLastFlag, &tree_items, &tree_count, &tree_full_paths);
		    refreshTreeSel(&ui, &state, tree_full_paths, tree_count, curr_path);
                    break;
		case 'c': // Clear clipboard contents
		    clearClipboard(clipboard_dir);
		    setStatus("Clipboard cleared");
		    break;
                case 'f': { // Search file
		    int selected_index = -1;
		    searchFile(curr_path, clipboard_dir, &file_system, &selected_index);
		    
		    refreshDirView(&dir_items, &dir_count, curr_path);
		    homeLastFlag = 1;
		    refreshTreeView(file_system.root, 0, &homeLastFlag, 
				   &tree_items, &tree_count, &tree_full_paths);
		    refreshTreeSel(&ui, &state, tree_full_paths, tree_count, curr_path);
		    
		    if (selected_index >= 0) {
			state.highlight = selected_index;
			
			// Ensure selected item is visible
			int win_height = getmaxy(ui.winDir) - 4;
			if (state.highlight < state.dir_top) {
			    state.dir_top = state.highlight;
			} else if (state.highlight >= state.dir_top + win_height) {
			    state.dir_top = state.highlight - win_height + 1;
			}
		    } else {
			state.highlight = 0;
			state.dir_top = 0;
		    }
		    break;
		}
		case 'u': // Undo
		    if (!stackEmpty(undo_stack)) {
			// Get operation type without accessing the node directly
			char operation = undo_stack.last_opt->opt;
			switch(operation) {
			    case 'C': undoCreate(&undo_stack, &redo_stack, &file_system); break;
			    case 'D': undoDelete(&undo_stack, &redo_stack, &file_system); break;
			    case 'P': undoPaste(&undo_stack, &redo_stack, &file_system); break;
			    case 'M': undoMove(&undo_stack, &redo_stack, &file_system); break;
			    case 'R': undoRename(&undo_stack, &redo_stack, &file_system); break;
			}
			setStatus("Undo operation performed");
			refreshDirView(&dir_items, &dir_count, curr_path);
			homeLastFlag = 1;
			refreshTreeView(file_system.root, 0, &homeLastFlag, &tree_items, &tree_count, &tree_full_paths);
			refreshTreeSel(&ui, &state, tree_full_paths, tree_count, curr_path);
		    }
		    break;
		case 'r': // Redo
		    if (!stackEmpty(redo_stack)) {
			// Get operation type without accessing the node directly
			char operation = redo_stack.last_opt->opt;
			switch(operation) {
			    case 'C': redoCreate(&redo_stack, &undo_stack, &file_system); break;
			    case 'D': redoDelete(&redo_stack, &undo_stack, &file_system); break;
			    case 'P': redoPaste(&redo_stack, &undo_stack, &file_system); break;
			    case 'M': redoMove(&redo_stack, &undo_stack, &file_system); break;
			    case 'R': redoRename(&undo_stack, &redo_stack, &file_system); break;
			}
			setStatus("Redo operation performed");
			refreshDirView(&dir_items, &dir_count, curr_path);
			homeLastFlag = 1;
			refreshTreeView(file_system.root, 0, &homeLastFlag, &tree_items, &tree_count, &tree_full_paths);
			refreshTreeSel(&ui, &state, tree_full_paths, tree_count, curr_path);
		    }
		    break;
	    }
        }

        updateAnimation();

        // Display header with truncation
        int max_header_width = COLS - 30;
        char header_path[PATH_MAX] = {0};
        if ((int)strlen(curr_path) > max_header_width) {
            strncpy(header_path, curr_path + strlen(curr_path) - max_header_width + 3, max_header_width);
            memmove(header_path, "...", 3);
            strcat(header_path, curr_path + strlen(curr_path) - max_header_width + 3);
        } else {
            strcpy(header_path, curr_path);
        }
        displayHeader(color_offset, header_path);
        
        if(state.IsSorted && dir_items) {
            char** sorted_dir = malloc(dir_count * sizeof(char*));
            sortDirectory(dir_items, dir_count, sorted_dir);
            DisplayDirWindow(&ui, sorted_dir, dir_count, state.highlight, state.dir_top);
            for (int i = 0; i < dir_count; i++) free(sorted_dir[i]);
            free(sorted_dir);
        } else {
            DisplayDirWindow(&ui, dir_items, dir_count, state.highlight, state.dir_top);
        }
       
        DisplayTreeWindow(&ui, tree_items, tree_count, state.highlight2, state.tree_top, tree_full_paths); 
        if(dir_count > 0 && state.highlight < dir_count) {
            char selected_path[PATH_MAX];
            if(state.IsSorted && sorted_dir) {
                // Gunakan sorted_dir jika sorting aktif
                snprintf(selected_path, PATH_MAX, "%s/%s", curr_path, sorted_dir[state.highlight]);
            } else {
                // Gunakan dir_items normal jika tidak sorting
                snprintf(selected_path, PATH_MAX, "%s/%s", curr_path, dir_items[state.highlight]);
            }
            DisplayInfoWindow(&ui, selected_path);
        } else {
            DisplayInfoWindow(&ui, curr_path);
        }

        refresh();
    }

    // Cleanup
    for(int i = 0; i < dir_count; i++) free(dir_items[i]);
    free(dir_items);
    
    for(int i = 0; i < tree_count; i++) {
        free(tree_items[i]);
        free(tree_full_paths[i]);
    }
    free(tree_items);
    free(tree_full_paths);

    if(sorted_dir) {
        for(int i = 0; i < dir_count; i++) {
            if(sorted_dir[i]) free(sorted_dir[i]);
        }
        free(sorted_dir);
    }

    deallocFileSystem(file_system.root);
    delURStack(&undo_stack);
    delURStack(&redo_stack);
    free(clipboard_dir);
    free(curr_path);
    cleaupWindows(&ui);
    cleanupUI();
    
    return 0;
}
