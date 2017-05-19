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

/* text user interface states */
#define TUI_EXIT    0
#define TUI_SHELL   1
#define TUI_HIST    2
#define TUI_RESIZE  3

/* serial connection states */
#define CON_ERROR       0
#define CON_DETACHED    1
#define CON_STARTING    2
#define CON_RECEIVE     3
#define CON_SEND        4

/* buffer sizes */
#define SHELL_BUF_SIZE  40 
#define SHELL_HIST_SIZE 99

/* fixed window dimensions */
#define W_TITLE_W           39
#define W_RECEIVE_W         40
#define W_TITLE_H           4
#define W_CON_STATUS_H      3
#define W_SHELL_H           3
#define W_WAVE_H            10


/* program states */
extern int tui_state;           /* state of the text user interface */
extern int con_state;           /* state of the serial connection */

/* connection + tui shared variables */
extern char shell_buffer[];
extern int shell_position;
extern int shell_send_flag;     /* set,reset by both tui.c and connection.c */
float wave[8];                  /* term0.wave[0-7], range -0.5 to 0.5 */

/* ncurses windows */
extern WINDOW *w_title;             
extern WINDOW *w_shell;
extern WINDOW *w_receive;
extern WINDOW *w_status;
extern WINDOW *w_pins;

#endif /* global_h */
