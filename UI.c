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

void resizeWindows(UIWindows *ui){
    getmaxyx(stdscr, ui->maxy, ui->maxx);

    int tree_width = ui->maxx * 0.3;
    int dir_width = ui->maxx - tree_width - 5;
    if (tree_width < 10) tree_width = 10;
    
    // Clean up existing windows
    if (ui->winTree) delwin(ui->winTree);
    if (ui->winDir) delwin(ui->winDir);
    if (ui->winDisp) delwin(ui->winDisp);

    // Create new windows
    ui->winTree = newwin(ui->maxy - 5, tree_width, 2, 3);
    ui->winDir = newwin(30, dir_width, 2, tree_width + 4);
    ui->winDisp = newwin(ui->maxy - 35, dir_width, 32, tree_width + 4);

    // Set window properties
    box(ui->winTree, 0, 0);
    box(ui->winDir, 0, 0);
    box(ui->winDisp, 0, 0);
    
    keypad(ui->winTree, TRUE);
    keypad(ui->winDir, TRUE);
    scrollok(ui->winTree, TRUE);
    scrollok(ui->winDir, TRUE);
}

void cleaupWindows(UIWindows *ui){
    if (ui->winTree) delwin(ui->winTree);
    if (ui->winDir) delwin(ui->winDir);
    if (ui->winDisp) delwin(ui->winDisp);
}

//Fungsi untuk display
void displayHeader(int color_offset){
    move(0,0);
    clrtoeol();
    printw("press 'q to quit");
    rainbowColor(stdscr, "==== WaSIFM ====", 0, 0, color_offset);
    refresh();
}

void DisplayTreeWindow(UIWindows *ui, char** choices, int num_choices, int highlight, int tree_top){
    werase(ui->winTree);
    box(ui->winTree, 0, 0);
    
    for (int j = 0; j < num_choices; j++) {
        if (j == highlight) {
            wattron(ui->winTree, A_REVERSE);
        }
        mvwprintw(ui->winTree, j + 2 - tree_top, 2, "%s", choices[j]);
        wattroff(ui->winTree, A_REVERSE);
    }
    wrefresh(ui->winTree);
}

void DisplayDirWindow(UIWindows *ui, char **dir_items, int item_count, int highlight, int dir_top) {
    werase(ui->winDir);
    box(ui->winDir, 0, 0);
    
    for (int i = 0; i < item_count; i++) {
        if (i == highlight) wattron(ui->winDir, A_REVERSE);
        mvwprintw(ui->winDir, i + 2 - dir_top, 2, "%s", dir_items[i]);
        wattroff(ui->winDir, A_REVERSE);/*  */
    }
    wrefresh(ui->winDir);
}

void DisplayInfoWindow(UIWindows *ui, char* selected_item){
    werase(ui->winDisp);
    box(ui->winDisp, 0, 0);
    mvwprintw(ui->winDisp, 1, 1, "Kamu memilih %s", selected_item);
    wrefresh(ui->winDisp);
}

void rainbowColor(WINDOW *win, const char *text, int y, int x, int offset){
    if(has_colors()){
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

//input handling

int handleTreeInput(UIWindows *ui, UIState *state, int num_choices) {
    int choice = wgetch(ui->winTree);
    
    switch (choice) {
        case 'j':
            if (--state->highlight2 < 0) state->highlight2 = 0;
            if (state->highlight2 < state->tree_top) state->tree_top = state->highlight2;
            break;
        case 'k':
            if (++state->highlight2 >= num_choices) state->highlight2 = num_choices - 1;
            if (state->highlight2 >= state->tree_top + (getmaxy(ui->winTree) - 4))
                state->tree_top = state->highlight2 - (getmaxy(ui->winTree) - 5);
            break;
        case 'l':
            ui->cur_window = ui->winDir;
            state->highlight = 0;
            state->dir_top = 0;
            break;
        case 'q':
            return 1; // Signal to quit
    }
    return 0;
}

int handleDirInput(UIWindows *ui, UIState *state, char **current_dir, char **sorted_dir, int current_count) {
    int choice = wgetch(ui->winDir);
    
    switch (choice) {
        case 'j':
            if (--state->highlight < 0) state->highlight = 0;
            if (state->highlight < state->dir_top) state->dir_top = state->highlight;
            break;
        case 'k':
            if (++state->highlight >= current_count) state->highlight = current_count - 1;
            if (state->highlight >= state->dir_top + (getmaxy(ui->winDir) - 4))
                state->dir_top = state->highlight - (getmaxy(ui->winDir) - 5);
            break;
        case 'h':
            ui->cur_window = ui->winTree;
            state->IsSorted = false;
            break;
        case 'o':
            if (!state->IsSorted) {
                // Free existing sorted data if any
                for (int i = 0; i < current_count; i++) {
                    if (sorted_dir[i]) {
                        free(sorted_dir[i]);
                        sorted_dir[i] = NULL;
                    }
                }
                // Create new sorted data
                sortDirectory(current_dir, current_count, sorted_dir);
            }
            state->IsSorted = !state->IsSorted;
            state->highlight = 0;
            state->dir_top = 0;
            break;
        case 'q':
            return 1; // Signal to quit
    }
    return 0;
}

//Directori operation
void sortDirectory(char **current_dir, int current_count, char **sorted_dir) {
    for (int i = 0; i < current_count; i++) {
        sorted_dir[i] = strdup(current_dir[i]);
        if (!sorted_dir[i]) {
            perror("strdup failed");
            exit(EXIT_FAILURE);
        }
    }
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