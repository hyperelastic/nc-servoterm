/*
 * file: tui.c
 *
 * text user interface
 *
 */


#include <ncurses.h>

#include "tui.h"
#include "global.h"


/* global text-user-interface-specific */
WINDOW *w_title;


/*TODO figure out why this works ok in xterm but awful in raw console mode */

void draw_cat() {
    mvwprintw(w_title, 1, 1, "NC-SERVOTERM, mode categories");
    mvwprintw(w_title, 3, 1, "Press any key to proceed, up/down for fun, F2 for"
            " exit");
}

void draw_pin() {
    mvwprintw(w_title, 1, 1, "NC-SERVOTERM, mode pins");
    mvwprintw(w_title, 3, 1, "Press any key to proceed, up/down for fun, F2 for"
            " exit");
}

void draw_input() {
    mvwprintw(w_title, 1, 1, "NC-SERVOTERM, mode input");
    mvwprintw(w_title, 3, 1, "Press any key to proceed, up/down for fun, F2 for"
            " exit");
}

void draw_exit() {
    mvwprintw(w_title, 1, 1, "NC-SERVOTERM, mode exit");
    mvwprintw(w_title, 3, 1, "Exiting. Press any key to quit the program.");
}

void draw_screen() {
    werase(w_title);
    switch(tui_state) {
        case TUI_CATEGORY:  draw_cat();     break;
        case TUI_PIN:       draw_pin();     break;
        case TUI_INPUT:     draw_input();   break;
        case TUI_EXIT:      draw_exit();    break;
    }
    box(w_title, 0, 0);
    wrefresh(w_title);
}

void input_cat(int key) {
    switch (key) {
        case KEY_F(1):  tui_state=TUI_EXIT;         break;
        case KEY_DOWN:  mvprintw(12, 10, "Down.");  break;
        case KEY_UP:    mvprintw(12, 10, "Up.");    break;
        default:        tui_state = TUI_PIN;        break;
    }
}

void input_pin(int key) {
    switch (key) {
        case KEY_F(1):  tui_state=TUI_EXIT;         break;
        case KEY_DOWN:  mvprintw(12, 10, "Down.");  break;
        case KEY_UP:    mvprintw(12, 10, "Up.");    break;
        default:        tui_state = TUI_INPUT;      break;
    }
}

void input_key(int key) {
    switch (key) {
        case KEY_F(1):  tui_state=TUI_EXIT;         break;
        case KEY_DOWN:  mvprintw(12, 10, "Down.");  break;
        case KEY_UP:    mvprintw(12, 10, "Up.");    break;
        default:        tui_state = TUI_EXIT;       break;
    }
}

void input_handle() {
    int key = getch();
    mvprintw(12, 0, "Up/Down:");
    switch(tui_state) {
        case TUI_CATEGORY:  input_cat(key);     break;
        case TUI_PIN:       input_pin(key);     break;
        case TUI_INPUT:     input_key(key);     break;
        case TUI_EXIT:                          break;
    }
    refresh();
}

void tui_setup() {
    initscr();
    keypad(stdscr, 1);
    cbreak();
    noecho();

    w_title = newwin(5, 80, 0, 0);
    refresh();

    /* stmbl status window, used by con_manager */
    w_con_status = newwin(3, 39, 5, 0);
    box(w_con_status, 0, 0);
    refresh();
    wrefresh(w_con_status);

    /* stmbl output window, used by con_reciever thread */
    w_con_receive = newwin(40, 40, 5, 40);
    box(w_con_receive, 0, 0);
    refresh();
    wrefresh(w_con_receive);
    mvwin(w_con_receive, 6,41);
    wresize(w_con_receive, 38, 38);
    scrollok(w_con_receive, TRUE);
    wclear(w_con_receive);
    wrefresh(w_con_receive);
}

void tui_cleanup() {
    erase();
    refresh();
    endwin();
}
