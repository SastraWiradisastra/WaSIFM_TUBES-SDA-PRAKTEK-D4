#include "UI.h"

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
void displayHeader(int color_offset);
void DisplayTreeWindow(UIWindows *ui, char** choices, int num_choices, int highlight, int tree_top);
void DisplayDirWindow(UIWindows *ui, char** choices, int num_choices, int highlight, int dir_top);
void DisplayInfoWindow(UIWindows *ui, char** selected_item);
void rainbowColor(UIWindows *ui, const char *text, int y, int x, int offset);

//input handling
int handleTreeInput(UIWindows *ui, UIState *state, int num_choices);
int handleDirInput(UIWindows *ui, UIState *state, char **current_dir, char **sorted_dir, int current_count);

//Directori operation
void sortDirectory(char **current_dir, int current_count, char **sorted_dir);
void freeDirectory(char **dir, int count);