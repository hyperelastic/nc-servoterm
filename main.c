#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ncurses.h>
#include <menu.h>
#include "hal_list.h"

#define NUMTHRDS 2

//www.youtube.com/watch?v=QdzcExCojZA
// +
//www.youtube.com/watch?v=vLto1EaiiUc thread mutexes

pthread_t t [NUMTHRDS];
int coin_flip;

pthread_mutex_t flip_done;

static void *thread2(void *_){
    /* once this is hit, block all new threads untill unlock */
    pthread_mutex_lock(&flip_done);
    printf("Thread 2: flipped coin %d\n", coin_flip);
}
static void *thread1(void *_){
    coin_flip = 23; //initialize a global variable
    printf("Thread 1: flipped coin %d\n", coin_flip);
    pthread_mutex_unlock(&flip_done);
}

int main() {
    pthread_mutex_init(&flip_done, NULL); // NULL for no pmutex attribute
    pthread_mutex_lock(&flip_done); // lock the main thread
    pthread_create(&t[1], NULL, thread2, NULL);
    pthread_create(&t[0], NULL, thread1, NULL);

    //must have this as main will block until all the supported threads are done
    pthread_mutex_destroy(&flip_done);
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
