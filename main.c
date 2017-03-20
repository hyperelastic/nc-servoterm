#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ncurses.h>
#include <menu.h>
#include "hal_list.h"

#define NUMTHRDS 2

//www.youtube.com/watch?v=QdzcExCojZA

pthread_t t [NUMTHRDS];
int coin_flip;

//pthread_mutex_t flip_done; //mutual exclusion

static void *thread2(void *_){
    printf("Thread 2: flipped coin %d\n", coin_flip);
}
static void *thread1(void *_){
    printf("Thread 1: flipped coin %d\n", coin_flip);
}

int main() {
    pthread_create(&t[1], NULL, thread2, NULL);
    pthread_create(&t[0], NULL, thread1, NULL);

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
