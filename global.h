/* 
 * file: global.h
 *
 * All the global variables.
 * 
 * This is a state machine. Some of the variables are shared in different files.
 *
 * You can access the mainly tui.c related states from the connection.c file,
 * but please do not change them. Also vice-versa.
 *
 */


#ifndef global_h
#define global_h


#include <ncurses.h>

/* text user interface states for main thread */
#define TUI_EXIT    0
#define TUI_SHELL   1
#define TUI_HIST    2 /* shell history. TODO receive history*/
#define TUI_PIN     3
#define TUI_RESIZE  4

/* connection states for connection threads */
#define CON_ERROR       0
#define CON_DETACHED    1
#define CON_STARTING    2
#define CON_CONNECTED   3

/* size of message[] */
#define SHELL_BUF_SIZE  40 
#define SHELL_HIST_SIZE 100

#define W_TITLE_W           39
#define W_CON_RECEIVE_W     40
#define W_TITLE_H           4
#define W_CON_STATUS_H      3
#define W_SHELL_H           3


/* program states */
extern int tui_state;           /* state of the text user interface */
extern int con_state;           /* state of the connection */

/* connection + tui shared variables */
extern char shell_buffer[];     /* write this to port */
extern int shell_position;      /* with this int */
extern int shell_send_flag;     /* send message to stmbl when true */

/* ncurses windows for tui */
extern WINDOW *w_title;             
extern WINDOW *w_shell;
extern WINDOW *w_con_receive;
extern WINDOW *w_con_status;
extern WINDOW *w_pins;

#endif /* global_h */
