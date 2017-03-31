/* 
 * file: global.h
 *
 * All the global variables.
 * 
 * There is the main thread, that deals with text user interface.
 * Second thread manages connection with stmbl + if connected sends out stuff.
 * Third thread (if STMBL connected) reads and parses it's output.
 *
 * This is a state machine. Some of the variables are shared between threads. 
 * You can access the tui states from the connection threads, but
 * please do not change them. Also vice-versa.
 *
 */


#ifndef global_h
#define global_h


#include <ncurses.h>
#include <pthread.h>

/* text user interface states for main thread */
#define TUI_EXIT     0
#define TUI_CATEGORY 1
#define TUI_PIN      2
#define TUI_INPUT    3

/* connection states for connection threads */
#define CON_ERROR       0
#define CON_DETACHED    1
#define CON_STARTING    2
#define CON_CONNECTED   3


/* program states */
extern int tui_state;           /* state of main(), for text user interface */
extern int con_state;           /* state of the connection-specific threads */

/* contains the connection threads */
extern pthread_t threads[2];    /* [0]=manage & send, [1]=receive */

/* connection-specific variables */
extern struct sp_port *port;
extern struct sp_port **ports;
extern char rx;                 /* read from port */
extern char message[];          /* write to port TODO dynamic */

/* ncurses windows for tui */
extern WINDOW *w_title;             
extern WINDOW *w_con_receive;   /* reserved for the receiver thread */
extern WINDOW *w_con_status;    /* reserved for the con_manager thread */


#endif /* global_h */
