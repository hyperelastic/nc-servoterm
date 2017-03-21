#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ncurses.h>
#include <menu.h>
#include <unistd.h> //sleep
#include "hal_list.h"

#define NUMTHRDS 2

pthread_t threads [NUMTHRDS];
int manager_state = 0;
int i;

static void *worker(void *_){
    mvprintw(5, 5, "\ti: %d\n", i);
    refresh();
    pthread_exit(NULL);
}

static void *manager(void *arg){
    int* pmanager_state = arg;
    while (1) {
        switch ( *pmanager_state ) {
            case 0: { // normal
                pthread_create(&threads[1], NULL, worker, NULL);
                ++i;
                usleep(1e6);
                break;
            }
            case -1: { // clean
                pthread_exit(NULL);
                break;
            }
            case 1: { // special
                usleep(1e6);
                break;
            }
        }
    }
}

int main() {
    int i = 0;
    int key;

    initscr();
    keypad(stdscr, 1);
    cbreak();
    noecho();
    refresh();
    mvprintw(1, 1, "Press any key, enter for exit.");

    pthread_create(&threads[0], NULL, manager, (void*)&manager_state);
    while (1) {
        key = getch();
        if (key==10) {
            wclear(stdscr);
            mvprintw(10, 10, "Exiting");
            refresh();
            manager_state = -1;
            break;
        }
        else {
            mvprintw(10, 10, "Your input was %c\n", key);
        }
    }
 
    pthread_exit(NULL);
    endwin();

    return 0;
}
