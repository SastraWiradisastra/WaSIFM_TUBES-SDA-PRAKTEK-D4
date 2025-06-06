#include <curses.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

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
    scrollok(*winTree, TRUE);  // Added scrolling
    scrollok(*winDir, TRUE);   // Added scrolling
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
    int highlight = 0;
    int highlight2 = 0;
    int tree_top = 0;  // Added scroll position
    int dir_top = 0;   // Added scroll position

    int color_offset = 0;
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
            tree_top = 0;  // Reset scroll on resize
            dir_top = 0;   // Reset scroll on resize
        }
        
        // Tree window display (unchanged except for scroll position)
        werase(winTree);
        box(winTree, 0, 0);
        int tree_max_items = getmaxy(winTree) - 3;
        for (int j = 0; j < num_choices2; j++) {
            if (j == highlight2) {
                wattron(winTree, A_REVERSE);
            }
            mvwprintw(winTree, j+2-tree_top, 2, "%s", Tchoices[j]);  // Added -tree_top
            wattroff(winTree, A_REVERSE);
        }
        wrefresh(winTree);
    
        if(cur_window == winDir) {
            // Directory window display (unchanged except for scroll position)
            werase(winDir);
            box(winDir, 0, 0);
            int dir_max_items = getmaxy(winDir) - 3;
            char** current_dir;
            int current_count;
            
            switch(highlight2) {
                case 0: current_dir = Dir1; current_count = num_choices1; break;
                case 1: current_dir = Dir2; current_count = num_choices3; break;
                case 2: current_dir = Dir3; current_count = num_choices4; break;
            }
            
            for (int i = 0; i < current_count; i++) {
                if (i == highlight) wattron(winDir, A_REVERSE);
                mvwprintw(winDir, i+2-dir_top, 2, "%s", current_dir[i]);  // Added -dir_top
                wattroff(winDir, A_REVERSE);
            }
            wrefresh(winDir);
            
            // Display selection in winDisp
            werase(winDisp);
            box(winDisp, 0, 0);
            mvwprintw(winDisp, 1, 1, "Kamu memilih %s", current_dir[highlight]);
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
                    highlight = 0;  // Reset highlight when switching
                    dir_top = 0;    // Reset scroll when switching
                    break;
                case 'q':
                    goto end_loop;
            }
        }
        
        napms(50);
    }
    end_loop:
    
    delwin(winTree);
    delwin(winDir);
    delwin(winDisp);
    endwin();
    return 0;
}