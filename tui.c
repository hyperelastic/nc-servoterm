/*
 * file: tui.c
 *
 * text user interface
 *
 */


#include <ncurses.h>
#include <ctype.h>
#include <string.h>

#include "tui.h"
#include "global.h"


/* global program states */
int tui_state;
int con_state;

/* global text-user-interface-specific */
WINDOW *w_title;
WINDOW *w_shell;
WINDOW *w_con_status;
WINDOW *w_con_receive;

/* global connection+display variables */
char shell_buffer[SHELL_BUF_SIZE] = "";
int shell_position = 0;
int shell_send_flag = 0;

/* local */


void draw_shell() {
    mvwprintw(w_title, 1, 1, "NC-SERVOTERM, mode shell");
    mvwprintw(w_title, 2, 1, "F5-console F6-category F7-pin F8-quit"
           " F9-stop F10-start F11-clear");
    mvwprintw(w_title, 3, 1, "Input command in shell and send with enter.");
    mvwprintw(w_shell, 1, 1, ">");
    mvwprintw(w_shell, 1, 3, shell_buffer);
}

void draw_cat() {
    mvwprintw(w_title, 1, 1, "NC-SERVOTERM, mode categories");
    mvwprintw(w_title, 2, 1, "F5-console F6-category F7-pin F8-quit F9-stop");
    mvwprintw(w_title, 3, 1, "Pick stmbl-hal category and press enter.");
}

void draw_pin() {
    mvwprintw(w_title, 1, 1, "NC-SERVOTERM, mode pins");
    mvwprintw(w_title, 2, 1, "F5-console F6-category F7-pin F8-quit F9-stop");
    mvwprintw(w_title, 3, 1, "Pick stmbl-hal pin and press enter.");
}

void draw_exit() {
    mvwprintw(w_title, 1, 1, "NC-SERVOTERM, mode exit");
    mvwprintw(w_title, 3, 1, "Exiting. Press any key to quit the program.");
}

void draw_con(char* description) {
    mvwprintw(w_con_status, 1, 1, "STMBL is %s.", description);
}

void draw_screen() {
    werase(w_title);
    werase(w_shell);
    werase(w_con_status);
    switch(tui_state) {
        case TUI_SHELL:     draw_shell();   break;
        case TUI_CATEGORY:  draw_cat();     break;
        case TUI_PIN:       draw_pin();     break;
        case TUI_EXIT:      draw_exit();    break;
    }
    switch(con_state) {
       case CON_DETACHED:   draw_con("detached");    break;
       case CON_STARTING:   draw_con("starting");    break;
       case CON_CONNECTED:  draw_con("connected");   break;
       case CON_ERROR:                               break;
    }
    box(w_title, 0, 0);
    box(w_shell, 0, 0);
    box(w_con_status, 0, 0);
    wnoutrefresh(w_con_receive); /* don't erase this win - it holds buffer */
    wnoutrefresh(w_con_status);
    wnoutrefresh(w_title);
    wnoutrefresh(w_shell);
    doupdate();
}

void shell_set_fault() {
    memset(shell_buffer, 0, sizeof(shell_buffer));
    strncpy(shell_buffer, "net0.enable=0", SHELL_BUF_SIZE);
    shell_position = 13;
    shell_send_flag = 1;
}

void shell_reset_fault() {
    memset(shell_buffer, 0, sizeof(shell_buffer));
    strncpy(shell_buffer, "net0.enable=1", SHELL_BUF_SIZE);
    shell_position = 13;
    shell_send_flag = 1;
}

void shell_clear() {
    memset(shell_buffer, 0, sizeof(shell_buffer));
    shell_position = 0;
    werase(w_con_receive);
}

void shell_back() {
    if (shell_position > 0) {
        shell_position--;
        shell_buffer[shell_position] = '\0';
    }
}

void shell_write(int input_key) {
    if (isprint(input_key)) {
        if (shell_position < SHELL_BUF_SIZE-1) {
            shell_buffer[shell_position] = input_key;
            shell_position++;
            shell_buffer[shell_position] = '\0';
        }
    }
//    else {
//    }
}

void shell_send() {
    shell_send_flag = 1; /* reset from connection.c when sent */
}

void input_shell(int key) {
    switch (key) {
        case KEY_F(8):      tui_state=TUI_EXIT;     break;
        case KEY_F(6):      tui_state=TUI_CATEGORY; break;
        case KEY_F(7):      tui_state=TUI_PIN;      break;
        case KEY_F(9):      shell_set_fault();      break;
        case KEY_F(10):     shell_reset_fault();    break;
        case KEY_F(11):     shell_clear();          break;
        case KEY_BACKSPACE: shell_back();           break;
        case 10 /*enter*/:  shell_send();           break;
//        case KEY_DOWN:  tui_state=TUI_PIN;          break; /* TODO history*/
//        case KEY_UP:    tui_state=TUI_CATEGORY;     break; /* TODO history*/
        default:            shell_write(key);        break;
    }
}

void input_cat(int key) { /* TODO HAL category menu */
    switch (key) {
        case KEY_F(8):      tui_state=TUI_EXIT;     break;
        case KEY_F(5):      tui_state=TUI_SHELL;    break;
        case KEY_F(7):      tui_state=TUI_PIN;      break;
        case KEY_F(9):      shell_set_fault();      break;
        case KEY_F(10):     shell_reset_fault();    break;
        case KEY_F(11):     shell_clear();          break;
        default:                                    break;
    }
}

void input_pin(int key) { /* TODO HAL pin menu */
    switch (key) {
        case KEY_F(8):      tui_state=TUI_EXIT;     break;
        case KEY_F(5):      tui_state=TUI_SHELL;    break;
        case KEY_F(6):      tui_state=TUI_CATEGORY; break;
        case KEY_F(9):      shell_set_fault();      break;
        case KEY_F(10):     shell_reset_fault();    break;
        case KEY_F(11):     shell_clear();          break;
        default:                                    break;
    }
}

void input_handle(int key) {
    switch(tui_state) {
        case TUI_SHELL:     input_shell(key);       break;
        case TUI_CATEGORY:  input_cat(key);         break;
        case TUI_PIN:       input_pin(key);         break;
        case TUI_EXIT:                              break;
    }
}

void tui_setup() {
    initscr();
    keypad(stdscr, 1);
    nodelay(stdscr, 1); /* important, for nonblocking getch() */
    cbreak();
    noecho();

    /* name of program + help */
    w_title = newwin(5, 80, 0, 0);
    refresh();
    box(w_title, 0, 0);
    wrefresh(w_title);

    /* shell for user input */
    w_shell = newwin(3, 39, 8, 0);
    refresh();
    box(w_shell, 0, 0);
    wrefresh(w_shell);

    /* stmbl connection status window */
    w_con_status = newwin(3, 39, 5, 0);
    refresh();
    box(w_con_status, 0, 0);
    wrefresh(w_con_status);

    /* stmbl connection output window, also accesed in connection.c */
    w_con_receive = newwin(40, 40, 5, 40);
    refresh();
    box(w_con_receive, 0, 0);
    wrefresh(w_con_receive);
    mvwin(w_con_receive, 6,41);
    wresize(w_con_receive, 38, 38);
    scrollok(w_con_receive, TRUE);
    wclear(w_con_receive);
    wrefresh(w_con_receive);
}

void tui_cleanup() {
    delwin(w_title);
    delwin(w_con_receive);
    erase();
    refresh();
    endwin();
}
