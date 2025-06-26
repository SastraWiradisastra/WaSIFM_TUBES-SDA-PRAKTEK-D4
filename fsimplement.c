#include "fsimplement.h"
#include "urimplement.h"

/*
 *	Core file system operations
 */

void openFile(char* path) {
	def_prog_mode();  // Save current ncurses state
	endwin();         // End ncurses mode temporarily

	pid_t pid = fork();
	if (pid == -1) {
		perror("fork failed");
		reset_prog_mode();
		refresh();
		return;
	}

	if (pid == 0) {
		execlp("xdg-open", "xdg-open", path, NULL); // Open regular file with default application
		perror("execlp failed");
		exit(EXIT_FAILURE);
	} else {
		int status;
		if (waitpid(pid, &status, 0) == -1) {
			perror("waitpid failed");
			exit(EXIT_FAILURE);
		}

		if (WEXITSTATUS(status) != 0) {
			perror("failure to exit xdg");
			exit(EXIT_FAILURE);	
		}
		
		reset_prog_mode();  // Restore saved ncurses state
		refresh();          // Refresh the screen
	}
}

void searchFile(char* curr_path, char* clipboard_dir, fsTree* fs, int* selected_index) {
    def_prog_mode();  // Save ncurses state
    endwin();         // End ncurses temporarily
    
    // Create temp file for fzf output
    char tmp_file[PATH_MAX];
    snprintf(tmp_file, PATH_MAX, "%s/fzf_result", clipboard_dir);
    
    pid_t pid = fork();
    if (pid == 0) {
        // Clear screen and run fzf without preview
        // Omit dot files/directories and the clipboard directory itself
        execlp("sh", "sh", "-c", 
               "clear; find \"$PWD\" ! -path \"$PWD\" "
               "! -path \"$1\" ! -path \"$1/*\" "
               "! -path '*/.*' -print 2>/dev/null | "
               "fzf --height=40% --reverse > \"$2\"", 
               "sh", clipboard_dir, tmp_file, NULL);
        perror("execlp failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            FILE* fp = fopen(tmp_file, "r");
            if (fp) {
                char selected[PATH_MAX];
                if (fgets(selected, PATH_MAX, fp)) {
                    // Remove newline
                    selected[strcspn(selected, "\n")] = 0;
                    
                    if (strlen(selected) > 0) {
                        // Check if it's a regular file
                        if (isRegFile(selected)) {
                            // Open file immediately without nesting ncurses calls
                            pid_t open_pid = fork();
                            if (open_pid == 0) {
                                execlp("xdg-open", "xdg-open", selected, NULL);
                                perror("execlp failed");
                                exit(EXIT_FAILURE);
                            } else if (open_pid > 0) {
                                int open_status;
                                waitpid(open_pid, &open_status, 0);
                            }
                            *selected_index = -1;  // No selection needed
                        } else {
                            // Handle directory
                            char* parent = getParentDir(selected);
                            if (parent) {
                                // Update current path if it's a directory
                                if (strcmp(curr_path, parent) != 0) {
                                    strcpy(curr_path, parent);
                                    
                                    // Update file system tree
                                    updateDirElmt(curr_path, fs->root);
                                }
                                
                                // Get filename for highlighting
                                char* filename = getFilename(selected);
                                
                                // Return index for highlighting
                                *selected_index = -1;  // Default: not found
                                
                                // Count directory items
                                int count = countDirElmt(curr_path);
                                if (count > 0) {
                                    // Get directory items
                                    char** items = malloc(count * sizeof(char*));
                                    DIR* dir = opendir(curr_path);
                                    if (dir) {
                                        struct dirent* entry;
                                        int i = 0;
                                        while ((entry = readdir(dir)) != NULL) {
                                            if (strcmp(entry->d_name, ".") != 0 && 
                                                strcmp(entry->d_name, "..") != 0 && 
                                                entry->d_name[0] != '.') {
                                                items[i] = strdup(entry->d_name);
                                                i++;
                                            }
                                        }
                                        closedir(dir);
                                        
                                        // Find matching index
                                        for (int j = 0; j < count; j++) {
                                            if (strcmp(items[j], filename) == 0) {
                                                *selected_index = j;
                                                break;
                                            }
                                        }
                                        
                                        // Cleanup
                                        for (int j = 0; j < count; j++) free(items[j]);
                                        free(items);
                                    }
                                }
                                free(parent);
                            }
                        }
                    }
                }
                fclose(fp);
            }
        }
        unlink(tmp_file);  // Cleanup temp file
    }
    
    reset_prog_mode();  // Restore ncurses
    refresh();
}

void createFile(char* path, fsTree* fs, optStack* undo_stack) {
	if (!path) {
		perror("Invalid paramters!!");
		exit(EXIT_FAILURE);
	}

	char* parent_dir = getParentDir(path);
	if (!parent_dir) {
		perror("Failed to get parent directory!");
		exit(EXIT_FAILURE);
	}	

	pid_t pid = fork();
	if (pid == -1) {
		perror("fork failed");
		free(parent_dir);
		return;
	}

	if (pid == 0) {
		execlp("touch", "touch", path, NULL);
		perror("execlp failed");
		exit(EXIT_FAILURE);
	} else {
		int status;
		waitpid(pid, &status, 0);

		if (WEXITSTATUS(status) == 0) {
			if (fs)
				updateDirElmt(parent_dir, fs->root);
			recordOperation(undo_stack, 'C', path, NULL);
		} else {
			perror("Failed to create file! Status error.");
			exit(EXIT_FAILURE);
		}
	}
	free(parent_dir);
}

void deleteFile(char* path, char* clipboard_dir, fsTree* fs, optStack* undo_stack) {
	// Copy the file to clipboard before deletion
	copyFile(path, clipboard_dir);

	// Now delete the original file
	pid_t rm_pid = fork();
	if (rm_pid == -1) {
		perror("fork failed");
		return;
	}

	if (rm_pid == 0) {
		// Child process for deletion
		execlp("rm", "rm", "-rf", path, NULL);
		perror("execlp failed");
		exit(EXIT_FAILURE);
	} else {
		// Parent process
		int status;
		if (waitpid(rm_pid, &status, 0) == -1) {
			perror("waitpid failed");
			return;
		}

		if (WEXITSTATUS(status) == 0) {
			// Update file system tree
			if (fs != NULL) {
				char* parent = getParentDir(path);
				if (parent != NULL) {
					updateDirElmt(parent, fs->root);
					free(parent);
				}
				recordOperation(undo_stack, 'D', path, clipboard_dir);
			}
		} else {
			perror("Failed to delete file! status error.");
			exit(EXIT_FAILURE);
		}
	}
}

void copyFile(char* path, char* clipboard_dir) {
	if (!path) {
		perror("Invalid parameters!");
		exit(EXIT_FAILURE);
	}

	// Copy to clipboard directory before deletion
	char* filename = getFilename(path);
	if (!filename) {
		perror("Could not extract filename");
		exit(EXIT_FAILURE);
	}

	// Set the clipboard path length
	size_t clipboard_path_len = strlen(clipboard_dir) + strlen(filename) + 2;
	char* clipboard_dest = malloc(clipboard_path_len);
	if (!clipboard_dest) {
		perror("Memory allocation failed");
		exit(EXIT_FAILURE);
	}

	// Write to the clipboard path
	snprintf(clipboard_dest, clipboard_path_len, "%s/%s", clipboard_dir, filename);

	// Copy to clipboard directory first
	pid_t cp_pid = fork();
	if (cp_pid == -1) {
		perror("fork failed");
		free(clipboard_dest);
		return;
	}

	if (cp_pid == 0) {
		// Child process for copying file
		execlp("cp", "cp", "-r", "-r", path, clipboard_dest, NULL);
		perror("execlp failed");
		exit(EXIT_FAILURE);
	} else {
		// Parent process
		int status;
		if (waitpid(cp_pid, &status, 0) == -1) {
			perror("waitpid failed");
			free(clipboard_dest);
			return;
		}

		if (WEXITSTATUS(status) != 0) {
			perror("Failed to copy file to clipboard. Status error.");
			exit(EXIT_FAILURE);
		}
	}

	free(clipboard_dest);
}

void moveFile(char* path, char* dest_path, fsTree* fs, optStack* undo_stack) {
	if (path == NULL || dest_path == NULL) {
		perror("Invalid paths!");
		exit(EXIT_FAILURE);
	}

	// Use mv command to move file/directory
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork failed");
		return;
	}

	if (pid == 0) {
		// Child process
		execlp("mv", "mv", path, dest_path, NULL);
		perror("execlp failed");
		exit(EXIT_FAILURE);
	} else {
		// Parent process
		int status;
		if (waitpid(pid, &status, 0) == -1) {
			perror("waitpid failed");
			return;
		}

		if (WEXITSTATUS(status) == 0) {
			// Update file system tree
			if (fs != NULL) {
				char* src_parent = getParentDir(path);
				char* dest_parent = getParentDir(dest_path);
				if (src_parent != NULL) {
					updateDirElmt(src_parent, fs->root);
					free(src_parent);
				}
				if (dest_parent != NULL) {
					updateDirElmt(dest_parent, fs->root);
					free(dest_parent);
				}
				recordOperation(undo_stack, 'M', path, dest_path);
			}
		} else {
			perror("Failed to move file! Status error.");
			exit(EXIT_FAILURE);
		}
	}
}

void renameFile(char* path, char* new_name, fsTree* fs, optStack* undo_stack) {
	if (path == NULL || new_name == NULL) {
		perror("Error: Invalid parameters\n");
		exit(EXIT_FAILURE);
	}

	// Construct new path
	char* parent = getParentDir(path);
	if (parent == NULL) {
		perror("Error: Could not determine parent directory\n");
		exit(EXIT_FAILURE);
	}

	size_t new_path_len = strlen(parent) + strlen(new_name) + 2;
	char* new_path = malloc(new_path_len);
	if (new_path == NULL) {
		perror("Error: Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}

	snprintf(new_path, new_path_len, "%s/%s", parent, new_name);
	free(parent);
	
	// Use mv command to rename (move to new name)
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork failed");
		free(new_path);
		return;
	}

	if (pid == 0) {
		// Child process
		execlp("mv", "mv", path, new_path, NULL);
		perror("execlp failed");
		exit(EXIT_FAILURE);
	} else {
		// Parent process
		int status;
		if (waitpid(pid, &status, 0) == -1) {
			perror("waitpid failed");
			free(new_path);
			return;
		}

		if (WEXITSTATUS(status) == 0) {
			// Update file system tree
			if (fs != NULL) {
				char* parent_dir = getParentDir(path);
				if (parent_dir != NULL) {
					updateDirElmt(parent_dir, fs->root);
					free(parent_dir);
				}
    				recordOperation(undo_stack, 'R', path, new_path); 
			}
		} else {
			perror("Failed to rename file! Status error.");
			exit(EXIT_FAILURE);
		}
	}

	free(new_path);
}

void clearClipboard(char* clipboard_dir) {
    if (!clipboard_dir) return;
    
    DIR* dir = opendir(clipboard_dir);
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        
        char path[PATH_MAX];
        snprintf(path, PATH_MAX, "%s/%s", clipboard_dir, entry->d_name);
        
        pid_t pid = fork();
        if (pid == 0) {
            execlp("rm", "rm", "-rf", path, NULL);
            perror("execlp failed");
            exit(EXIT_FAILURE);
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
    closedir(dir);
}

void pasteFile(char* dest_dir, char* clipboard_dir, fsTree* fs, optStack* undo_stack) {
    if (!dest_dir || !clipboard_dir) {
        perror("Invalid parameters");
        return;
    }

    // Buffer so that it doesn't copy the entire directory
    char src_buf[PATH_MAX];
    snprintf(src_buf, sizeof(src_buf), "%s/.", clipboard_dir);

    // Copy clipboard contents to destination
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return;
    }

    if (pid == 0) {
        execlp("cp", "cp", "-r", src_buf, dest_dir, NULL);
        perror("execlp failed");
        exit(EXIT_FAILURE);
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            return;
        }

        if (WEXITSTATUS(status) == 0) {
            // Update file system tree
            if (fs) updateDirElmt(dest_dir, fs->root);
	    recordOperation(undo_stack, 'P', dest_dir, clipboard_dir);
        } else {
            perror("Failed to paste files. Status error.");
	    exit(EXIT_FAILURE);
        }
    }
}
