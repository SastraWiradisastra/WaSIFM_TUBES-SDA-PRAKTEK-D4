#ifndef UI_H
#define UI_H

#include "structure.h"
#include "fsimplement.h"
#include "helper.h"
// #include "urimplement.h"
#include "base.h"

typedef struct {
    WINDOW *winTree;
    WINDOW *winDir;
    WINDOW *winDisp;
    WINDOW *cur_window;
    int maxx, maxy;
} UIWindows;

typedef struct {
    int highlight;
    int highlight2;
    int tree_top;
    int dir_top;
    int color_offset;
    bool IsSorted;
} UIState;

//Fungsi untuk deklarasi
void initUI(void);
void cleanupUI(void);
void initWindows(UIWindows *ui);
void resizeWindows(UIWindows *ui);
void cleaupWindows(UIWindows *ui);

//Fungsi untuk display
void displayHeader(int color_offset, const char* selected_item);
void DisplayTreeWindow(UIWindows *ui, char** choices, int num_choices, int highlight, int tree_top, char** tree_full_paths);
void DisplayDirWindow(UIWindows *ui, char** choices, int num_choices, int highlight, int dir_top);
void DisplayInfoWindow(UIWindows *ui, char* selected_item);
void updateWindowBorders(UIWindows *ui);
void rainbowColor(WINDOW *win, const char *text, int y, int x, int offset);

//Directori operation
void sortDirectory(char **current_dir, int current_count, char **sorted_dir);
void freeSortedDir(char **dir, int count);

#endif
