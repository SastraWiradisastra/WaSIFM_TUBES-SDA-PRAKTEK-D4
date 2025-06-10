#include "fsimplement.h"

/*
 *	Core file system operations
 */

void openFile(char* path, char* curr_path) {
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
		// Change to directory if it's a directory
		if (curr_path != NULL) {
			free(curr_path);
			curr_path = strdup(path);
		}
		printf("Changed directory to: %s\n", path);
		return;
	}

	// Open regular file with default application
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

void searchFile(void) {
	/* fzf implementation pending */
}

void createFile(char* path, fsTree* fs) {
	if (path == NULL) {
		printf("Error: Invalid path\n");
		return;
	}

	if (pathExists(path) == 0) {
		printf("File already exists: %s\n", path);
		return;
	}

	char* parent_dir = getParentDirectory(path);
	if (parent_dir != NULL) {
		if (dirPermissions(parent_dir) == 2) {
			printf("You don't have permissions to create files in: %s\n", parent_dir);
			free(parent_dir);
			return;
		}
		free(parent_dir);
	}

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
			
			// Update file system tree
			if (fs != NULL) {
				char* parent = getParentDirectory(path);
				if (parent != NULL) {
					updateDirElmt(parent, fs->root);
					free(parent);
				}
			}
		} else {
			printf("Failed to create file: %s\n", path);
		}
	}
}

void deleteFile(char* path, char* clipboard_dir, fsTree* fs) {
	if (path == NULL || clipboard_dir == NULL) {
		printf("Error: Invalid parameters\n");
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

	// Copy to clipboard directory before deletion
	char* filename = getFilename(path);
	if (filename == NULL) {
		printf("Error: Could not extract filename\n");
		return;
	}

	size_t clipboard_path_len = strlen(clipboard_dir) + strlen(filename) + 2;
	char* clipboard_dest = malloc(clipboard_path_len);
	if (clipboard_dest == NULL) {
		printf("Error: Memory allocation failed\n");
		return;
	}

	snprintf(clipboard_dest, clipboard_path_len, "%s/%s", clipboard_dir, filename);

	// Copy to clipboard directory first
	pid_t cp_pid = fork();
	if (cp_pid == -1) {
		perror("fork failed");
		free(clipboard_dest);
		return;
	}

	if (cp_pid == 0) {
		// Child process for copy
		execlp("cp", "cp", "-r", path, clipboard_dest, NULL);
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
			printf("Failed to copy file to clipboard: %s\n", path);
			free(clipboard_dest);
			return;
		}
	}

	free(clipboard_dest);

	// Now delete the original file
	pid_t rm_pid = fork();
	if (rm_pid == -1) {
		perror("fork failed");
		return;
	}

	if (rm_pid == 0) {
		// Child process for deletion
		execlp("trash-put", "trash-put", path, NULL);
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
			printf("File deleted and moved to trash: %s\n", path);

			// Update file system tree
			if (fs != NULL) {
				char* parent = getParentDirectory(path);
				if (parent != NULL) {
					updateDirElmt(parent, fs->root);
					free(parent);
				}
			}
		} else {
			printf("Failed to delete file: %s\n", path);
		}
	}
}

void copyFile(char* path, char* clipboard_dir, fsTree* fs) {
	if (path == NULL || clipboard_dir == NULL) {
		printf("Error: Invalid parameters\n");
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

	if (dirPermissions(clipboard_dir) == 2) {
		printf("You don't have permissions to write to clipboard directory: %s\n", clipboard_dir);
		return;
	}

	char* filename = getFilename(path);
	if (filename == NULL) {
		printf("Error: Could not extract filename\n");
		return;
	}

	size_t clipboard_path_len = strlen(clipboard_dir) + strlen(filename) + 2;
	char* clipboard_dest = malloc(clipboard_path_len);
	if (clipboard_dest == NULL) {
		printf("Error: Memory allocation failed\n");
		return;
	}

	snprintf(clipboard_dest, clipboard_path_len, "%s/%s", clipboard_dir, filename);

	// Use cp command to copy file/directory to clipboard
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork failed");
		free(clipboard_dest);
		return;
	}

	if (pid == 0) {
		// Child process
		execlp("cp", "cp", "-r", path, clipboard_dest, NULL);
		perror("execlp failed");
		exit(EXIT_FAILURE);
	} else {
		// Parent process
		int status;
		if (waitpid(pid, &status, 0) == -1) {
			perror("waitpid failed");
			free(clipboard_dest);
			return;
		}

		if (WEXITSTATUS(status) == 0) {
			printf("File copied to clipboard: %s\n", path);
		} else {
			printf("Failed to copy file to clipboard: %s\n", path);
		}
	}

	free(clipboard_dest);
}

void moveFile(char* path, char* dest_path, fsTree* fs) {
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
	if (dest_parent != NULL) {
		if (dirPermissions(dest_parent) == 2) {
			printf("You don't have permissions to write to: %s\n", dest_parent);
			free(dest_parent);
			return;
		}
		free(dest_parent);
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
			printf("File moved from %s to %s\n", path, dest_path);

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
		} else {
			printf("Failed to move file from %s to %s\n", path, dest_path);
		}
	}
}

void renameFile(char* path, char* new_name, fsTree* fs) {
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

			// Update file system tree
			if (fs != NULL) {
				char* parent_dir = getParentDirectory(path);
				if (parent_dir != NULL) {
					updateDirElmt(parent_dir, fs->root);
					free(parent_dir);
				}
			}
		} else {
			printf("Failed to rename file from %s to %s\n", path, new_path);
		}
	}

	free(new_path);
}

void pasteFile(char* dest_dir, char* clipboard_dir, fsTree* fs) {
	if (dest_dir == NULL || clipboard_dir == NULL) {
		printf("Error: Invalid parameters\n");
		return;
	}

	if (pathExists(dest_dir) != 0) {
		printf("Destination directory does not exist: %s\n", dest_dir);
		return;
	}

	if (isRegFile(dest_dir)) {
		printf("Destination is not a directory: %s\n", dest_dir);
		return;
	}

	if (dirPermissions(dest_dir) == 2) {
		printf("You don't have permissions to write to: %s\n", dest_dir);
		return;
	}

	if (pathExists(clipboard_dir) != 0) {
		printf("Clipboard directory does not exist: %s\n", clipboard_dir);
		return;
	}

	// Check if clipboard directory is empty
	DIR* dir = opendir(clipboard_dir);
	if (dir == NULL) {
		printf("Cannot open clipboard directory: %s\n", clipboard_dir);
		return;
	}

	struct dirent* entry;
	bool has_files = false;
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
			has_files = true;
			break;
		}
	}
	closedir(dir);

	if (!has_files) {
		printf("Clipboard is empty\n");
		return;
	}

	// Copy all files from clipboard to destination directory
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork failed");
		return;
	}

	if (pid == 0) {
		// Child process to copy all contents from clipboard to destination
		char* clipboard_pattern = malloc(strlen(clipboard_dir) + 3);
		if (clipboard_pattern == NULL) {
			printf("Error: Memory allocation failed\n");
			exit(EXIT_FAILURE);
		}
		snprintf(clipboard_pattern, strlen(clipboard_dir) + 3, "%s/*", clipboard_dir);
		
		execlp("cp", "cp", "-r", clipboard_pattern, dest_dir, NULL);
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
			printf("Files pasted from clipboard to: %s\n", dest_dir);

			// Clear clipboard directory after successful paste
			pid_t clear_pid = fork();
			if (clear_pid == -1) {
				perror("fork failed for clipboard clear");
				return;
			}

			if (clear_pid == 0) {
				// Child process to clear clipboard
				char* clipboard_pattern = malloc(strlen(clipboard_dir) + 3);
				if (clipboard_pattern == NULL) {
					printf("Error: Memory allocation failed\n");
					exit(EXIT_FAILURE);
				}
				snprintf(clipboard_pattern, strlen(clipboard_dir) + 3, "%s/*", clipboard_dir);
				
				execlp("rm", "rm", "-rf", clipboard_pattern, NULL);
				perror("execlp failed");
				exit(EXIT_FAILURE);
			} else {
				// Parent process
				int clear_status;
				if (waitpid(clear_pid, &clear_status, 0) == -1) {
					perror("waitpid failed for clipboard clear");
					return;
				}

				if (WEXITSTATUS(clear_status) == 0) {
					printf("Clipboard cleared\n");
				} else {
					printf("Warning: Failed to clear clipboard\n");
				}
			}

			// Update file system tree
			if (fs != NULL) {
				updateDirElmt(dest_dir, fs->root);
			}
		} else {
			printf("Failed to paste files to: %s\n", dest_dir);
		}
	}
}

void clearTrash(void) {
	if (trash_path == NULL) {
		printf("Error: Trash path not initialized\n");
		return;
	}

	// Use trash-empty command first 
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork failed");
		return;
	}

	if (pid == 0) {
		// Child process - try trash-empty first
		execlp("trash-empty", "trash-empty", NULL);
		perror("clear trash failed");
		exit(EXIT_FAILURE);
	} else {
		// Parent process
		int status;
		if (waitpid(pid, &status, 0) == -1) {
			perror("waitpid failed");
			return;
		}

		if (WEXITSTATUS(status) == 0) {
			printf("Trash cleared successfully\n");
		} else {
			printf("Failed to clear trash\n");
		}
	}
}

void restoreFromTrash(char* filename) {
	if (filename == NULL) {
		printf("Error: Invalid filename\n");
		return;
	}

	if (trash_path == NULL) {
		printf("Error: Trash path not initialized\n");
		return;
	}

	// Use trash-restore command first
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork failed");
		return;
	}

	if (pid == 0) {
		// Child process
		execlp("trash-restore", "trash-restore", filename, NULL);
		perror("trash-restore failed");
		exit(EXIT_FAILURE);
	} else {
		// Parent process
		int status;
		if (waitpid(pid, &status, 0) == -1) {
			perror("waitpid failed");
			return;
		}

		if (WEXITSTATUS(status) == 0) {
			printf("File restored from trash: %s\n", filename);
		} else {
			printf("Failed to restore file from trash: %s\n", filename);
		}
	}
}
