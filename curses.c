#include <curses.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>

int compare_strings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void DirSort(char** current_dir, int current_count, char** sorted_dir){
    for(int i = 0; i < current_count; i++){
        sorted_dir[i] = strdup(current_dir[i]);  
        if (!sorted_dir[i]) {
            perror("strdup failed");
            exit(EXIT_FAILURE);
        }
    }
    qsort(sorted_dir, current_count, sizeof(char*), compare_strings);
}

void rainbowcolor(WINDOW *win, const char *text, int y, int x, int offset) {
    if(!has_colors()) {
        mvprintw(y, x, "%s", text);
        return;
    }

    int colors[] = {1, 2, 3, 4, 5, 6};
    int len = strlen(text);

    for(int i = 0; i < len; i++) {
        int color_idx = (i + offset) % 6;
        wattron(win, COLOR_PAIR(colors[color_idx]));
        mvwaddch(win, y, x + i, text[i]);
        wattroff(win, COLOR_PAIR(colors[color_idx]));
    }
    wrefresh(win);
}

void resize_windows(WINDOW **winTree, WINDOW **winDir, WINDOW **winDisp) {
    int maxx, maxy;
    getmaxyx(stdscr, maxy, maxx);

    int tree_width = maxx * 0.3;  
    int dir_width = maxx - tree_width - 5; 
    
    if (*winTree) delwin(*winTree);
    if (*winDir) delwin(*winDir);
    if (*winDisp) delwin(*winDisp);

    *winTree = newwin(maxy-5, tree_width, 2, 3);
    *winDir = newwin(30, dir_width, 2, tree_width + 4);
    *winDisp = newwin(maxy-35, dir_width, 32, tree_width + 4);

    box(*winTree, 0, 0);
    box(*winDir, 0, 0);
    box(*winDisp, 0, 0);
    
    keypad(*winTree, TRUE);
    keypad(*winDir, TRUE);
    scrollok(*winTree, TRUE);
    scrollok(*winDir, TRUE);
}

void free_sorted_dir(char** dir, int count) {
    if (dir) {
        for (int i = 0; i < count; i++) {
            if (dir[i]) {  // Check if pointer is not NULL before freeing
                free(dir[i]);
            }
        }
        free(dir);
    }
}

int main() {
    initscr();
    noecho();
    cbreak();
    curs_set(0);

    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(1, COLOR_RED, -1);
        init_pair(2, COLOR_YELLOW, -1);
        init_pair(3, COLOR_GREEN, -1);
        init_pair(4, COLOR_CYAN, -1);
        init_pair(5, COLOR_BLUE, -1);
        init_pair(6, COLOR_MAGENTA, -1);
    }

    int maxx, maxy;
    getmaxyx(stdscr, maxy, maxx);

    WINDOW *winTree = NULL;
    WINDOW *winDir = NULL;
    WINDOW *winDisp = NULL;
    WINDOW *cur_window = NULL;

    resize_windows(&winTree, &winDir, &winDisp);
    cur_window = winTree;

    int old_maxx, old_maxy;
    getmaxyx(stdscr, old_maxy, old_maxx);
    
    char* Tchoices[] = {"Dir1", "Dir2", "Dir3"};
    char* Dir1[] = {"Amalia", "Devi", "Diaz", "Wahyu"};
    char* Dir2[] = {"Rafi", "Afriza", "Dino", "Fauzan", "Hadi","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28"};
    char* Dir3[] = {"Zaidan", "Johan", "Darrel", "Naufal"};
    int choice;
    int num_choices1 = sizeof(Dir1)/sizeof(char*);
    int num_choices3 = sizeof(Dir2)/sizeof(char*);
    int num_choices4 = sizeof(Dir3)/sizeof(char*);    
    int num_choices2 = sizeof(Tchoices)/sizeof(char*); 
    
    // Initialize sorted directory arrays with NULL pointers
    char** sorted_dir1 = calloc(num_choices1, sizeof(char*));
    char** sorted_dir2 = calloc(num_choices3, sizeof(char*));
    char** sorted_dir3 = calloc(num_choices4, sizeof(char*));
    
    // Check for allocation failures
    if (!sorted_dir1 || !sorted_dir2 || !sorted_dir3) {
        fprintf(stderr, "Memory allocation failed\n");
        if (sorted_dir1) free(sorted_dir1);
        if (sorted_dir2) free(sorted_dir2);
        if (sorted_dir3) free(sorted_dir3);
        endwin();
        return 1;
    }
    
    int highlight = 0;
    int highlight2 = 0;
    int tree_top = 0;
    int dir_top = 0;

    int color_offset = 0;
    bool IsSorted = false;
    
    printw("Press 'q' to quit");
    
    while(1) {
        move(0, 0);
        clrtoeol();

        rainbowcolor(stdscr, "==== WasiFM ====", 0, 0, color_offset);
        color_offset = (color_offset + 1) % 6;
        
        refresh();
        int new_maxx, new_maxy;
        getmaxyx(stdscr, new_maxy, new_maxx);
        
        if (new_maxx != old_maxx || new_maxy != old_maxy) {
            resize_windows(&winTree, &winDir, &winDisp);
            cur_window = winTree;
            flushinp();
            old_maxx = new_maxx;
            old_maxy = new_maxy;
            tree_top = 0;
            dir_top = 0;
        }
        
        // Tree window display
        werase(winTree);
        box(winTree, 0, 0);
        int tree_max_items = getmaxy(winTree) - 3;
        for (int j = 0; j < num_choices2; j++) {
            if (j == highlight2) {
                wattron(winTree, A_REVERSE);
            }
            mvwprintw(winTree, j+2-tree_top, 2, "%s", Tchoices[j]);  
            wattroff(winTree, A_REVERSE);
        }
        wrefresh(winTree);
    
        if(cur_window == winDir) {
            werase(winDir);
            box(winDir, 0, 0);
            int dir_max_items = getmaxy(winDir) - 3;
            char** current_dir;
            char** sorted_dir;
            int current_count;
            
            switch(highlight2) {
                case 0: 
                    current_dir = Dir1; 
                    sorted_dir = sorted_dir1; 
                    current_count = num_choices1;
                    break;
                case 1: 
                    current_dir = Dir2; 
                    sorted_dir = sorted_dir2; 
                    current_count = num_choices3;
                    break;
                case 2: 
                    current_dir = Dir3; 
                    sorted_dir = sorted_dir3; 
                    current_count = num_choices4;
                    break;
            }

            // Check if sorted array has been allocated by checking first element
            bool is_allocated = (sorted_dir[0] != NULL);
            char** display_dir = (IsSorted && is_allocated) ? sorted_dir : current_dir;
            for (int i = 0; i < current_count; i++) {
                if (i == highlight) wattron(winDir, A_REVERSE);
                mvwprintw(winDir, i+2-dir_top, 2, "%s", display_dir[i]);  
                wattroff(winDir, A_REVERSE);
            }
            wrefresh(winDir);
            
            // Display selection in winDisp
            werase(winDisp);
            box(winDisp, 0, 0);
            mvwprintw(winDisp, 1, 1, "Kamu memilih %s", display_dir[highlight]);
            wrefresh(winDisp);
            
            // Input handling with scroll position adjustment
            choice = wgetch(winDir);
            switch(choice) {
                case 'j':
                    if (--highlight < 0) highlight = 0;
                    if (highlight < dir_top) dir_top = highlight;
                    break;
                case 'k':
                    if (++highlight >= current_count) highlight = current_count-1;
                    if (highlight >= dir_top + (getmaxy(winDir)-4)) 
                        dir_top = highlight - (getmaxy(winDir)-5);
                    break;
                case 'h':
                    cur_window = winTree;
                    IsSorted = false;
                    break;
                case 'o':
                    if (!IsSorted) {
                        // Free existing sorted data if any
                        for (int i = 0; i < current_count; i++) {
                            if (sorted_dir[i]) {
                                free(sorted_dir[i]);
                                sorted_dir[i] = NULL;
                            }
                        }
                        // Create new sorted data
                        DirSort(current_dir, current_count, sorted_dir);
                    }
                    IsSorted = !IsSorted;  
                    highlight = 0;  
                    dir_top = 0;
                    break;
                case 'q':
                    goto end_loop;
            }
        }
        else {  // winTree is current window
            choice = wgetch(winTree);
            switch(choice) {
                case 'j':
                    if (--highlight2 < 0) highlight2 = 0;
                    if (highlight2 < tree_top) tree_top = highlight2;
                    break;
                case 'k':
                    if (++highlight2 >= num_choices2) highlight2 = num_choices2-1;
                    if (highlight2 >= tree_top + (getmaxy(winTree)-4))
                        tree_top = highlight2 - (getmaxy(winTree)-5);
                    break;
                case 'l':
                    cur_window = winDir;
                    highlight = 0;  
                    dir_top = 0;    
                    break;
                case 'q':
                    goto end_loop;
            }
        }
        
        napms(50);
    }
    
    end_loop:
    // Clean up sorted directories - check each array dynamically
    for (int i = 0; i < num_choices1; i++) {
        if (sorted_dir1[i]) free(sorted_dir1[i]);
    }
    for (int i = 0; i < num_choices3; i++) {
        if (sorted_dir2[i]) free(sorted_dir2[i]);
    }
    for (int i = 0; i < num_choices4; i++) {
        if (sorted_dir3[i]) free(sorted_dir3[i]);
    }
    
    // Free the arrays themselves
    free(sorted_dir1);
    free(sorted_dir2);
    free(sorted_dir3);

    // Clean up windows
    if (winTree) delwin(winTree);
    if (winDir) delwin(winDir);
    if (winDisp) delwin(winDisp);
    
    endwin();
    return 0;
}