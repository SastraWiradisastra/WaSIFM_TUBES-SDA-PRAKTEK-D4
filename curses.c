#include <curses.h>
#include <string.h>

int main(){

    //Membuat sebuah window (alokasi memori)
    initscr();
    noecho();
    cbreak();

    int maxx, maxy;
    getmaxyx(stdscr, maxy, maxx);

    WINDOW *winTree = newwin(50, 60, 1, 0);
    WINDOW *winDir = newwin(30, maxx-65, 1, 65);
    WINDOW *winDisp = newwin(18, maxx-65, 31, 65);
    WINDOW *cur_window = winTree;
    box(winTree, 0, 0);
    box(winDir, 0, 0);
    box(winDisp, 0, 0);        
    refresh();

    wrefresh(winTree);
    wrefresh(winDir);
    wrefresh(winDisp);
    keypad(winDir, true);
    
    //Menampilkan teks di window
    
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

    while(1)
    {
        for (int j = 0; j < num_choices2; j++)
        {
            if (j == highlight2){
                wattron(winTree, A_REVERSE);
            }
            mvwprintw(winTree, j+1, 1, "%s\n", Tchoices[j]);
            wattroff(winTree, A_REVERSE);
        }
    
        if(cur_window == winDir){
        choice = 0;
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

            if (choice == 10){
                werase(winDisp);
                box(winDisp, 0, 0);

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
            }

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
                        mvwprintw(winDir, i+1, 1, "%s\n", Dir1[i]);
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
                        mvwprintw(winDir, i+1, 1, "%s\n", Dir2[i]);
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
                        mvwprintw(winDir, i+1, 1, "%s\n", Dir3[i]);
                        wattroff(winDir, A_REVERSE);
                    }
                    
                } 
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

            if (choice == 10){
                if(highlight2 == 0)
                {
                    highlight = 0;
                    cur_window = winDir;
                    for (int i = 0; i < num_choices1; i++)
                    {
                        if (i == highlight){
                            wattron(winDir, A_REVERSE);
                        }
                        mvwprintw(winDir, i+1, 1, "%s\n", Dir1[i]);
                        wattroff(winDir, A_REVERSE);
                    }
                    
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
                        mvwprintw(winDir, i+1, 1, "%s\n", Dir2[i]);
                        wattroff(winDir, A_REVERSE);
                    }
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
                        mvwprintw(winDir, i+1, 1, "%s\n", Dir3[i]);
                        wattroff(winDir, A_REVERSE);
                    }
                    
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
    }
    //terminate window (dealokasi memori)
    delwin(winTree);
    delwin(winDir);
    delwin(winDisp);
    endwin();

    return 0;
}
