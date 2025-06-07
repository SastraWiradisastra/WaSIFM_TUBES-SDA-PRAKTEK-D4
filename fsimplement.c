#include "fsimplement.h"

/*
 *	Core file system operations
 */

// TODO: Make compatible with both regular files and directories
void openFile(char* path) {
	if (path == NULL) {
		printf("Error: Invalid path\n");
		return;
	}

	if (dirPermissions(path) == 2) {
		printf("You don't have permissions to access the file: %s\n", path);
		return;
	}

	if (pathExists(path) != 0) {
		printf("File does not exist: %s\n", path);
		return;
	}

	// Check if it's a directory
	if (!isRegFile(path)) {
		// Revise to open directory instead
		printf("Cannot open directory as file: %s\n", path);
		return;
	} else {
		// Use xdg-open to open the file with default application
		pid_t pid = fork();
		if (pid == -1) {
			perror("fork failed");
			return;
		}

		if (pid == 0) {
			// Child process
			execlp("xdg-open", "xdg-open", path, NULL);
			perror("execlp failed");
			exit(EXIT_FAILURE);
		} else {
			// Parent process
			int status;
			if (waitpid(pid, &status, 0) == -1) {
				perror("waitpid failed");
				return;
			}

			if (WEXITSTATUS(status) != 0) 
				printf("Failed to open file: %s\n", path);
		}
	}

}

void selectFile(char* path, char*** selected_files, int* selected_count) {
	if (path == NULL || selected_files == NULL || selected_count == NULL) {
		printf("Error: Invalid parameters\n");
		return;
	}

	if (pathExists(path) != 0) {
		printf("File does not exist: %s\n", path);
		return;
	}

	if (isSelected(path, *selected_files, *selected_count)) {
		printf("File already selected: %s\n", path);
		return;
	}

	addToSelection(path, selected_files, selected_count);
	printf("File selected: %s\n", path);
}

void searchFile() {
	/* fzf implementation pending */
}

void createFile(char* path, fsTree* fs, optStack* undo_stack, optStack* redo_stack) {
	if (path == NULL) {
		printf("Error: Invalid path\n");
		return;
	}

	if (pathExists(path) == 0) {
		printf("File already exists: %s\n", path);
		return;
	}

	char* parent_dir = getParentDirectory(path);
	if (parent_dir != NULL && dirPermissions(parent_dir) == 2) {
		printf("You don't have permissions to create files in: %s\n", parent_dir);
		free(parent_dir);
		return;
	}
	free(parent_dir);

	// Use touch command to create file
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork failed");
		return;
	}

	if (pid == 0) {
		// Child process
		execlp("touch", "touch", path, NULL);
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
			printf("File created: %s\n", path);
			if (undo_stack != NULL) 
				addToUndoStack(path, 'n', undo_stack, redo_stack);
			

			// Update file system tree
			if (fs != NULL) {
				char* parent = getParentDirectory(path);
				if (parent != NULL) {
				    updateDirElmt(parent, fs->root);
				    free(parent);
				}
			}
		} else 
			printf("Failed to create file: %s\n", path);
	}
}

// TODO: Replace with trash-cli
void deleteFile(char* path, fsTree* fs, optStack* undo_stack, optStack* redo_stack) {
	if (path == NULL) {
		printf("Error: Invalid path\n");
		return;
	}

	if (pathExists(path) != 0) {
		printf("File does not exist: %s\n", path);
		return;
	}

	if (dirPermissions(path) == 2) {
		printf("You don't have permissions to delete: %s\n", path);
		return;
	}

	// Use rm command to delete file/directory
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork failed");
		return;
	}

	if (pid == 0) {
		// Child process
		execlp("rm", "rm", "-rf", path, NULL);
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
			printf("File deleted: %s\n", path);
			if (undo_stack != NULL) 
				addToUndoStack(path, 'd', undo_stack, redo_stack);

			// Update file system tree
			if (fs != NULL) {
				char* parent = getParentDirectory(path);
				if (parent != NULL) {
					updateDirElmt(parent, fs->root);
					free(parent);
				}
			}
		} else 
			printf("Failed to delete file: %s\n", path);
	}
}

// TODO: Function supposed to copy to clipboard path
void copyFile(char* path, char* dest_path, fsTree* fs, optStack* undo_stack, optStack* redo_stack) {
	if (path == NULL || dest_path == NULL) {
		printf("Error: Invalid paths\n");
		return;
	}

	if (pathExists(path) != 0) {
		printf("Source file does not exist: %s\n", path);
		return;
	}

	if (dirPermissions(path) == 2) {
		printf("You don't have permissions to read: %s\n", path);
		return;
	}

	char* dest_parent = getParentDirectory(dest_path);
	if (dest_parent != NULL && dirPermissions(dest_parent) == 2) {
		printf("You don't have permissions to write to: %s\n", dest_parent);
		free(dest_parent);
		return;
	}
	free(dest_parent);

	// Use cp command to copy file/directory
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork failed");
		return;
	}

	if (pid == 0) {
		// Child process
		execlp("cp", "cp", "-r", path, dest_path, NULL);
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
			printf("File copied from %s to %s\n", path, dest_path);
			if (undo_stack != NULL) 
				addToUndoStack(dest_path, 'y', undo_stack, redo_stack);
			

			// Update file system tree
			if (fs != NULL) {
				char* parent = getParentDirectory(dest_path);
				if (parent != NULL) {
					updateDirElmt(parent, fs->root);
					free(parent);
				}
			}
		} else 
			printf("Failed to copy file from %s to %s\n", path, dest_path);
	}
}

// NOTE: This one should already work but should be reworked anyways 
void moveFile(char* path, char* dest_path, fsTree* fs, optStack* undo_stack, optStack* redo_stack) {
	if (path == NULL || dest_path == NULL) {
		printf("Error: Invalid paths\n");
		return;
	}

	if (pathExists(path) != 0) {
		printf("Source file does not exist: %s\n", path);
		return;
	}

	if (dirPermissions(path) == 2) {
		printf("You don't have permissions to move: %s\n", path);
		return;
	}

	char* dest_parent = getParentDirectory(dest_path);
	if (dest_parent != NULL && dirPermissions(dest_parent) == 2) {
		printf("You don't have permissions to write to: %s\n", dest_parent);
		free(dest_parent);
		return;
	}
	free(dest_parent);

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
			printf("File moved from %s to %s\n", path, dest_path);
			if (undo_stack != NULL) 
				addToUndoStack(path, 'm', undo_stack, redo_stack);

			// Update file system tree
			if (fs != NULL) {
				char* src_parent = getParentDirectory(path);
				char* dest_parent = getParentDirectory(dest_path);
				if (src_parent != NULL) {
					updateDirElmt(src_parent, fs->root);
					free(src_parent);
				}
				if (dest_parent != NULL) {
					updateDirElmt(dest_parent, fs->root);
					free(dest_parent);
				}
			}
		} else 
			printf("Failed to move file from %s to %s\n", path, dest_path);
	}
}

// NOTE: Potentially won't work with the current tree data structure
// (Something about this function also feels off)
void renameFile(char* path, char* new_name, fsTree* fs, optStack* undo_stack, optStack* redo_stack) {
	if (path == NULL || new_name == NULL) {
		printf("Error: Invalid parameters\n");
		return;
	}

	if (pathExists(path) != 0) {
		printf("File does not exist: %s\n", path);
		return;
	}

	if (dirPermissions(path) == 2) {
		printf("You don't have permissions to rename: %s\n", path);
		return;
	}

	// Construct new path
	char* parent = getParentDirectory(path);
	if (parent == NULL) {
		printf("Error: Could not determine parent directory\n");
		return;
	}

	size_t new_path_len = strlen(parent) + strlen(new_name) + 2;
	char* new_path = malloc(new_path_len);
	if (new_path == NULL) {
		printf("Error: Memory allocation failed\n");
		free(parent);
		return;
	}

	snprintf(new_path, new_path_len, "%s/%s", parent, new_name);
	free(parent);

	if (pathExists(new_path) == 0) {
		printf("File with new name already exists: %s\n", new_path);
		free(new_path);
		return;
	}

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
			printf("File renamed from %s to %s\n", path, new_path);
			if (undo_stack != NULL) 
				addToUndoStack(path, 'a', undo_stack, redo_stack);

			// Update file system tree
			if (fs != NULL) {
				char* parent_dir = getParentDirectory(path);
				if (parent_dir != NULL) {
					updateDirElmt(parent_dir, fs->root);
					free(parent_dir);
				}
			}
		} else 
			printf("Failed to rename file from %s to %s\n", path, new_path);
	}

	free(new_path);
}

// TODO: Cut file should be deleted
void cutFile(char* path, char** clipboard_path, bool* clipboard_is_cut) {
	if (path == NULL || clipboard_path == NULL || clipboard_is_cut == NULL) {
		printf("Error: Invalid parameters\n");
		return;
	}

	if (pathExists(path) != 0) {
		printf("File does not exist: %s\n", path);
		return;
	}

	if (*clipboard_path != NULL) 
		free(*clipboard_path);

	*clipboard_path = strdup(path);
	*clipboard_is_cut = true;
	printf("File cut to clipboard: %s\n", path);
}

// TODO: Revise this weird ass function (especially the parameter passing)
void pasteFile(char* dest_dir, char* clipboard_path, bool* clipboard_is_cut, fsTree* fs, optStack* undo_stack, optStack* redo_stack) {
	if (dest_dir == NULL || clipboard_path == NULL || clipboard_is_cut == NULL) {
		printf("Error: Invalid parameters\n");
		return;
	}

	if (clipboard_path == NULL) {
		printf("No file in clipboard\n");
		return;
	}

	if (pathExists(clipboard_path) != 0) {
		printf("Clipboard file no longer exists: %s\n", clipboard_path);
		return;
	}

	if (pathExists(dest_dir) != 0) {
		printf("Destination directory does not exist: %s\n", dest_dir);
		return;
	}

	char* filename = getFilename(clipboard_path);
	size_t dest_path_len = strlen(dest_dir) + strlen(filename) + 2;
	char* dest_path = malloc(dest_path_len);
	if (dest_path == NULL) {
	printf("Error: Memory allocation failed\n");
	return;
	}

	snprintf(dest_path, dest_path_len, "%s/%s", dest_dir, filename);

	if (*clipboard_is_cut) {
		moveFile(clipboard_path, dest_path, fs, undo_stack, redo_stack);
		*clipboard_is_cut = false;
	} else 
		copyFile(clipboard_path, dest_path, fs, undo_stack, redo_stack);
	
	free(dest_path);
}

