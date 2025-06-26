#include "UI.h"

//function bantuan untuk qsort
static int compare_strings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

//Fungsi untuk deklarasi
void initUI(void){
    initscr();
    noecho();
    cbreak();
    curs_set(0);

    if (has_colors()){
        start_color();
        use_default_colors();
        init_pair(1, COLOR_RED, -1);
        init_pair(2, COLOR_YELLOW, -1);
        init_pair(3, COLOR_GREEN, -1);
        init_pair(4, COLOR_CYAN, -1);
        init_pair(5, COLOR_BLUE, -1);
        init_pair(6, COLOR_MAGENTA, -1);       
    }
}

void cleanupUI(void){
    endwin();
}

void initWindows(UIWindows *ui){
    getmaxyx(stdscr, ui->maxy, ui->maxx);

    ui->winTree = NULL;
    ui->winDir = NULL;
    ui->winDisp = NULL;
    ui->cur_window = NULL;

    resizeWindows(ui);
    ui->cur_window = ui->winTree;
}

void resizeWindows(UIWindows *ui) {
    erase();
    refresh();
    
    // Remember current window type
    int was_tree = (ui->cur_window == ui->winTree);
    int was_dir = (ui->cur_window == ui->winDir);
    int was_disp = (ui->cur_window == ui->winDisp);

    if (ui->winTree) delwin(ui->winTree);
    if (ui->winDir) delwin(ui->winDir);
    if (ui->winDisp) delwin(ui->winDisp);

    getmaxyx(stdscr, ui->maxy, ui->maxx);
    
    int tree_width = (ui->maxx * 0.5) - 5;
    int dir_width = ui->maxx - tree_width - 5;
    if (tree_width < 10) tree_width = 10;
    
    ui->winTree = newwin(ui->maxy - 5, tree_width, 2, 3);
    ui->winDir = newwin(46, dir_width, 2, tree_width + 4);
    ui->winDisp = newwin(ui->maxy - 51, dir_width, 48, tree_width + 4);
    
    wattron(ui->winTree, COLOR_PAIR(4));
    wattron(ui->winDir, COLOR_PAIR(4));
    wattron(ui->winDisp, COLOR_PAIR(4));
    
    box(ui->winTree, 0, 0);
    box(ui->winDir, 0, 0); 
    box(ui->winDisp, 0, 0);

    keypad(ui->winTree, TRUE);
    keypad(ui->winDir, TRUE);
    scrollok(ui->winTree, TRUE);
    scrollok(ui->winDir, TRUE);
    
    // Restore current window
    if (was_tree) ui->cur_window = ui->winTree;
    else if (was_dir) ui->cur_window = ui->winDir;
    else if (was_disp) ui->cur_window = ui->winDisp;
    else ui->cur_window = ui->winTree; // Default
    
    clearok(stdscr, TRUE);
    redrawwin(stdscr);
    refresh();
    updateWindowBorders(ui);
}

void cleaupWindows(UIWindows *ui){
    if (ui->winTree) delwin(ui->winTree);
    if (ui->winDir) delwin(ui->winDir);
    if (ui->winDisp) delwin(ui->winDisp);
}

//Fungsi untuk display
void displayHeader(int color_offset, const char *selected_item) {
    static char last_path[PATH_MAX] = {0};  // Simpan state terakhir
    static int last_color_offset = -1;
    
    // Hanya update jika ada perubahan
    if(strcmp(last_path, selected_item) != 0 || last_color_offset != color_offset) {
        move(0,0);
        clrtoeol();
        
        // Print quit instruction
        printw("--> Press 'q' to quit <--");
        
        // Print WaSIFM
        int center_pos = (COLS - strlen("=======(WaSIFM)=======")) / 2;
        rainbowColor(stdscr, "=======(WaSIFM)=======", 0, center_pos, color_offset);
        
        // Print path hanya jika berubah
        if(selected_item && *selected_item) {
            int path_pos = COLS - strlen(selected_item) - 1;
            if(path_pos > center_pos + (int)strlen("=======(WaSIFM)=======")) {
                mvprintw(0, path_pos, "%s", selected_item);
            }
            strncpy(last_path, selected_item, PATH_MAX-1);
        }
        
        last_color_offset = color_offset;
    }
}

void DisplayTreeWindow(UIWindows *ui, char** choices, int num_choices, int highlight, int tree_top, char** tree_full_paths) {
    // Gunakan window buffer untuk mengurangi flickering
    WINDOW *buffer = newwin(ui->maxy - 5, getmaxx(ui->winTree), 2, 3);
   
    if(ui->cur_window == ui->winTree) wattron(buffer, COLOR_PAIR(1));
    box(buffer, 0, 0);
    if(ui->cur_window == ui->winTree) wattroff(buffer, COLOR_PAIR(1));
    
    int win_width = getmaxx(buffer);
    int max_display_width = win_width - 4;
    
    for (int j = 0; j < num_choices; j++) {
        int row = j + 2 - tree_top;
        if (row < 1 || row >= getmaxy(buffer) - 1) continue;
        
        char* full_path = tree_full_paths[j];
        char* display_text = choices[j];
        
        // Highlight jika perlu
        if (j == highlight) {
            wattron(buffer, A_REVERSE);
        }
        
        // Temukan posisi awal nama (setelah hierarchy markers)
        int name_pos = 0;
        while (display_text[name_pos] == ' ' || 
               display_text[name_pos] == '|' ||
               display_text[name_pos] == '+' || 
               display_text[name_pos] == '-' ||
               display_text[name_pos] == '>') {
            name_pos++;
        }
        
        // Cetak hierarchy markers dengan warna default
        mvwprintw(buffer, row, 2, "%.*s", name_pos, display_text);
        
        // Cetak nama dengan warna biru jika direktori
        if (!isRegFile(full_path)) {
            wattron(buffer, COLOR_PAIR(5));
        }
        
        // Cetak nama file/direktori
        if ((int)strlen(display_text + name_pos) > max_display_width - name_pos - 2) {
            char buf[max_display_width - name_pos - 2 + 1];
            strncpy(buf, display_text + name_pos, max_display_width - name_pos - 5);
            buf[max_display_width - name_pos - 5] = '\0';
            strcat(buf, "...");
            mvwprintw(buffer, row, 2 + name_pos, "%s", buf);
        } else {
            mvwprintw(buffer, row, 2 + name_pos, "%s", display_text + name_pos);
        }
        
        // Reset atribut
        if (!isRegFile(full_path)) {
            wattroff(buffer, COLOR_PAIR(5));
        }
        if (j == highlight) {
            wattroff(buffer, A_REVERSE);
        }
    }
    
    // Copy buffer ke window asli
    overwrite(buffer, ui->winTree);
    delwin(buffer);
    wrefresh(ui->winTree);
}

void DisplayDirWindow(UIWindows *ui, char **dir_items, int item_count, int highlight, int dir_top) {
    werase(ui->winDir);
    updateWindowBorders(ui);  
    wattroff(ui->winDir, COLOR_PAIR(1));
    int win_width = getmaxx(ui->winDir);
    int max_display_width = win_width - 4;  // Account for borders and padding
    
    for (int i = 0; i < item_count; i++) {
        int row = i + 2 - dir_top;
        if (row < 1 || row >= getmaxy(ui->winDir) - 1) continue;
        
        if (i == highlight) wattron(ui->winDir, A_REVERSE);
        
        // Check if the current item is a directory
        if (!isRegFile(dir_items[i])) {
            // It's a directory - apply directory color (blue)
            if (has_colors()) {
                wattron(ui->winDir, COLOR_PAIR(5)); // Blue color for directories
            }
        } 

        // Truncate long file names
        if ((int)strlen(dir_items[i]) > max_display_width) {
            char buf[max_display_width + 1];
            strncpy(buf, dir_items[i], max_display_width - 3);
            buf[max_display_width - 3] = '\0';
            strcat(buf, "...");
            mvwprintw(ui->winDir, row, 2, "%s", buf);
        } else {
            mvwprintw(ui->winDir, row, 2, "%s", dir_items[i]);
        }
        
        wattroff(ui->winDir, COLOR_PAIR(5));
        wattroff(ui->winDir, A_REVERSE);
    }
    wrefresh(ui->winDir);
}

void DisplayInfoWindow(UIWindows *ui, char* selected_item) {
    werase(ui->winDisp);
    box(ui->winDisp, 0, 0);
    wattroff(ui->winDisp, COLOR_PAIR(1));

    struct stat file_stat;
    if (stat(selected_item, &file_stat)) {
        mvwprintw(ui->winDisp, 1, 1, "Error getting info for: %s", selected_item);
        wrefresh(ui->winDisp);
        return;
    }

    // Extract just the filename
    char* filename = strrchr(selected_item, '/');
    filename = filename ? filename + 1 : selected_item;

    // Basic file info - now showing just filename instead of full path
    mvwprintw(ui->winDisp, 1, 1, "File: %s", filename);
    
    // Get file size and reformat using formatSize() in helper.c
    char* size_str = formatSize(file_stat.st_size);
    mvwprintw(ui->winDisp, 2, 1, "Size: %s", size_str);
    free(size_str);

    // File type
    char *file_type = "Unknown";
    if (S_ISREG(file_stat.st_mode)) file_type = "Regular File";
    else if (S_ISDIR(file_stat.st_mode)) file_type = "Directory";
    else if (S_ISLNK(file_stat.st_mode)) file_type = "Symbolic Link";
    else if (S_ISCHR(file_stat.st_mode)) file_type = "Character Device";
    else if (S_ISBLK(file_stat.st_mode)) file_type = "Block Device";
    else if (S_ISFIFO(file_stat.st_mode)) file_type = "FIFO/Named Pipe";
    else if (S_ISSOCK(file_stat.st_mode)) file_type = "Socket";
    mvwprintw(ui->winDisp, 3, 1, "Type: %s", file_type);

    // Permissions (unchanged)
    char permissions[] = "---------";
    permissions[0] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';
    permissions[3] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';
    permissions[6] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';
    mvwprintw(ui->winDisp, 4, 1, "Permissions: %s", permissions);

    // Time information (unchanged)
    char time_buf[80];
    struct tm *timeinfo = localtime(&file_stat.st_mtime);
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", timeinfo);
    mvwprintw(ui->winDisp, 5, 1, "Last Modified: %s", time_buf);

    wrefresh(ui->winDisp);
}

void rainbowColor(WINDOW *win, const char *text, int y, int x, int offset){
    if(!has_colors()){
        mvwprintw(win, y,x,"%s", text);
        return;
    }

    int colors[] = {1, 2, 3, 4, 5, 6};
    int len = strlen(text);

    for (int i = 0; i < len; i++) {
        int color_idx = (i + offset) % 6;
        wattron(win, COLOR_PAIR(colors[color_idx]));
        mvwaddch(win, y, x + i, text[i]);
        wattroff(win, COLOR_PAIR(colors[color_idx]));
    }
    wrefresh(win);
}

//Directori operation
void sortDirectory(char **current_dir, int current_count, char **sorted_dir) {
    // Copy items to sorted_dir
    for(int i = 0; i < current_count; i++) {
        sorted_dir[i] = strdup(current_dir[i]);
    }

    // Sort the array
    qsort(sorted_dir, current_count, sizeof(char*), compare_strings);
}

void freeSortedDir(char **dir, int count) {
    if (dir) {
        for (int i = 0; i < count; i++) {
            if (dir[i]) {
                free(dir[i]);
            }
        }
        free(dir);
    }
}

void updateWindowBorders(UIWindows *ui) {
    // Hanya update border tanpa menghapus konten
    if (ui->cur_window == ui->winTree) {
        wattron(ui->winTree, COLOR_PAIR(1));
        box(ui->winTree,0,0);
        wattroff(ui->winTree, COLOR_PAIR(1));
	box(ui->winDir,0,0);
    }
    else if (ui->cur_window == ui->winDir) {
        wattron(ui->winDir, COLOR_PAIR(1));
        box(ui->winDir,0,0);
        wattroff(ui->winDir, COLOR_PAIR(1));
	box(ui->winTree,0,0);
    }
}

