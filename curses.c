#include <curses.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

void rainbowcolor(WINDOW *win, const char *text, int y, int x, int offset){
    if(!has_colors()){
        mvprintw(y, x, "%s", text);
        return;
    }

    int colors[] = {1, 2, 3, 4, 5, 6};
    int len = strlen(text);

    for(int i = 0; i < len; i++){
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

    // Calculate widths 
    int tree_width = maxx * 0.3;  
    int dir_width = maxx - tree_width - 5; 
    
    // Delete old windows if they exist
    if (*winTree) delwin(*winTree);
    if (*winDir) delwin(*winDir);
    if (*winDisp) delwin(*winDisp);

    // Create new windows with adjusted sizes
    *winTree = newwin(maxy-5, tree_width, 2, 3);
    *winDir = newwin(30, dir_width, 2, tree_width + 4);
    *winDisp = newwin(maxy-35, dir_width, 32, tree_width + 4);

    // Draw borders
    box(*winTree, 0, 0);
    box(*winDir, 0, 0);
    box(*winDisp, 0, 0);
    
    // Enable keypad
    keypad(*winTree, TRUE);
    keypad(*winDir, TRUE);
}

int main(){

    //Membuat sebuah window (alokasi memori)
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
    
    //Deklarasi variabel
    
    char* Tchoices[] = {"Dir1", "Dir2", "Dir3"};
    char* Dir1[] = {"Amalia", "Devi", "Diaz", "Wahyu"};
    char* Dir2[] = {"Rafi", "Afriza", "Dino", "Fauzan", "Hadi"};
    char* Dir3[] = {"Zaidan", "Johan", "Darrel", "Naufal"};
    int choice;
    int num_choices1 = sizeof(Dir1)/sizeof(char*);
    int num_choices3 = sizeof(Dir2)/sizeof(char*);
    int num_choices4 = sizeof(Dir3)/sizeof(char*);    
    int num_choices2 = sizeof(Tchoices)/sizeof(char*); 
    int highlight = 0;
    int highlight2 = 0;


    int color_offset = 0;
    printw("Press 'q' to quit");
    while(1)
    {
        move(0, 0);
        clrtoeol();

        // Animate rainbow text
        rainbowcolor(stdscr, "==== WasiFM ====", 0, 0, color_offset);
        color_offset = (color_offset + 1) % 6;
        
        refresh();
        int new_maxx, new_maxy;
        getmaxyx(stdscr, new_maxy, new_maxx);
        
        if (new_maxx != old_maxx || new_maxy != old_maxy) {
            resize_windows(&winTree, &winDir, &winDisp);
            // Reset current window pointer to valid window
            cur_window = winTree;
            flushinp();  // Membersihkan buffer input
            old_maxx = new_maxx;
            old_maxy = new_maxy;
        }
        
        for (int j = 0; j < num_choices2; j++)
        {
            if (j == highlight2){
                wattron(winTree, A_REVERSE);
            }
            mvwprintw(winTree, j+1, 2, "%s\n", Tchoices[j]);
            wattroff(winTree, A_REVERSE);
            wrefresh(winTree);
        }
    
        if(cur_window == winDir){
        wmove(winDir, 1, 1);  // Pindahkan kursor ke posisi input
        wrefresh(winDir);
            switch(highlight2){
                case(0):
                    mvwprintw(winDisp, 1, 1, "Kamu memilih %s", Dir1[highlight]);
                    break;
                case(1):
                    mvwprintw(winDisp, 1, 1,"Kamu memilih %s", Dir2[highlight]);
                    break;
                case(2):
                    mvwprintw(winDisp, 1, 1,"Kamu memilih %s", Dir3[highlight]);
                    break;  
                default:
                    break;  
            }
                
        wrefresh(winDisp);
        choice = wgetch(winDir);
            int max_numchoice;
            if (highlight2 == 0){
                max_numchoice = num_choices1;
            } else if (highlight2 == 1){
                max_numchoice = num_choices3;
            } else {
                max_numchoice = num_choices4;
            }
            switch(choice)
            {
                case ('j'):
                    highlight--;
                    if(highlight < 0)
                    {
                        highlight = 0;
                    }
                    break;
                case ('k'):
                    highlight++;
                    if(highlight + 1 > max_numchoice)
                    {
                        highlight = max_numchoice - 1;
                    }
                    break;
                case ('h'):
                    cur_window = winTree;
                    break;
                default:
                    break;
            }

            
            werase(winDisp);
            box(winDisp, 0, 0);

            if(choice == 'q'){
                break;
            }

            werase(winDir);
            box(winDir, 0, 0);

                if(highlight2 == 0)
                {
                    for (int i = 0; i < num_choices1; i++)
                    {
                        if (i == highlight){
                            wattron(winDir, A_REVERSE);
                        }
                        mvwprintw(winDir, i+1, 2, "%s\n", Dir1[i]);
                        wattroff(winDir, A_REVERSE);
                        
                    }
                }    
                else if(highlight2 == 1)
                {
                    for (int i = 0; i < num_choices3; i++)
                    {
                        if (i == highlight){
                            wattron(winDir, A_REVERSE);
                        }
                        mvwprintw(winDir, i+1, 2, "%s\n", Dir2[i]);
                        wattroff(winDir, A_REVERSE);
                        
                    }
                }  
                else if(highlight2 == 2)
                {
                    for (int i = 0; i < num_choices4; i++)
                    {
                        if (i == highlight){
                            wattron(winDir, A_REVERSE);
                        }
                        mvwprintw(winDir, i+1, 2, "%s\n", Dir3[i]);
                        wattroff(winDir, A_REVERSE);
                    
                    }
                }
                wrefresh(winDir); 
        }

        if(cur_window == winTree){  
            choice = wgetch(winTree);

            switch(choice)
            {
                case ('j'):
                    highlight2--;
                    if(highlight2 < 0)
                    {
                        highlight2 = 0;
                    }
                    break;
                case ('k'):
                    highlight2++;
                    if(highlight2 + 1 > num_choices2)
                    {
                        highlight2 = num_choices2 - 1;
                    }
                    break;
                case('l'):
                    cur_window = winDir;
                    break;
                default:
                    break;
            }

            if (choice == 10 || choice == 'l'){
                if(highlight2 == 0)
                {
                    highlight = 0;
                    cur_window = winDir;
                    for (int i = 0; i < num_choices1; i++)
                    {
                        if (i == highlight){
                            wattron(winDir, A_REVERSE);
                        }
                        mvwprintw(winDir, i+1, 2, "%s\n", Dir1[i]);
                        wattroff(winDir, A_REVERSE);
                    }
                    wrefresh(winDir);
                    
                }    
                if(highlight2 == 1)
                {
                    highlight = 0;
                    cur_window = winDir;
                    for (int i = 0; i < num_choices3; i++)
                    {
                        if (i == highlight){
                            wattron(winDir, A_REVERSE);
                        }
                        mvwprintw(winDir, i+1, 2, "%s\n", Dir2[i]);
                        wattroff(winDir, A_REVERSE);
                    }
                    wrefresh(winDir);
                }  
                if(highlight2 == 2)
                {
                    highlight = 0;
                    cur_window = winDir;
                    for (int i = 0; i < num_choices4; i++)
                    {
                        if (i == highlight){
                            wattron(winDir, A_REVERSE);
                        }
                        mvwprintw(winDir, i+1, 2, "%s\n", Dir3[i]);
                        wattroff(winDir, A_REVERSE);
                    }
                    wrefresh(winDir);
                    
                }  
            }
            
            if(choice == 'q'){
                break;
            }

            werase(winTree);
            box(winTree, 0, 0);
        }
        if (choice == 'q'){
            break;
        }
        wrefresh(winTree);
        wrefresh(winDir);
        wrefresh(winDisp);
        napms(100);
    }
    //terminate window (dealokasi memori)
    delwin(winTree);
    delwin(winDir);
    delwin(winDisp);
    endwin();

    return 0;
}