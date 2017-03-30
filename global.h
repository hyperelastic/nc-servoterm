/* All the global variables */
#ifndef global_h
#define global_h

#include <ncurses.h>
#include <pthread.h>

/* ncurses display states for main thread */
#define NC_EXIT     0
#define NC_CATEGORY 1
#define NC_PIN      2
#define NC_INPUT    3

/* connection states for helper threads */
#define CON_ERROR       0
#define CON_DETACHED    1
#define CON_STARTING    2
#define CON_CONNECTED   3


extern pthread_t threads[2];    /* connection: [0]=manage & send, [1]=receive */
extern struct sp_port *port;
extern struct sp_port **ports;
extern char rx;                 /* read from port */
extern char message[];          /* write to port TODO dynamic */
extern WINDOW *w_title;             
extern WINDOW *w_con_receive;   /* reserved for the receiver thread */
extern WINDOW *w_con_status;    /* reserved for the con_manager thread */
extern int nc_state;
extern int con_state;

#endif
