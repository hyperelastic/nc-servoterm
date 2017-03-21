#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ncurses.h>
#include <menu.h>
#include <unistd.h> //sleep
#include "hal_list.h"

#define NUMTHRDS 2

pthread_t threads [NUMTHRDS];
int i;

static void *worker(void *_){
    /* once this is hit, block all new threads untill unlock */
    int j;
    for (j=0; j<5; ++j) {
        printf("\tj: %d\n", j);
        usleep(1e5);
    }
}
static void *manager(void *_){
    while (1) {
        printf("i: %d", i);
        pthread_create(&threads[1], NULL, worker, NULL);
        usleep(1e6);
        pthread_join(threads[1], NULL);
        i++;
    }
}

int main() {
    pthread_create(&threads[0], NULL, manager, NULL);

    //must have this as main will block until all the supported threads are done
    pthread_exit(NULL);

//    initscr();
//    cbreak();
//    noecho();
//
//    printw("asdf");
//
//    refresh();
//    getch();
//    endwin();
//
    return 0;
}
