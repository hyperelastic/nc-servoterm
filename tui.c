/*
 * file: tui.c
 *
 * text user interface
 *
 */


#include <ncurses.h>
#include <menu.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "tui.h"
#include "global.h"
#include "hal_list.h"


/* global program states */
int tui_state;
int con_state;

/* global text-user-interface-specific */
WINDOW *w_title;
WINDOW *w_shell;
WINDOW *w_status;
WINDOW *w_receive;
WINDOW *w_pins;

/* global connection+display variables */
char shell_buffer[SHELL_BUF_SIZE] = "";
int shell_position = 0;
int shell_send_flag = 0;

/* local */
int hist_i = 0;
char hist[SHELL_HIST_SIZE][SHELL_BUF_SIZE];


ITEM **construct_menu_items(char *items_list[], int n_items) {
    ITEM **items;
    int i;
    items = (ITEM **)calloc(n_items, sizeof(ITEM*));
    for(i=0; i<n_items; ++i) {
        items[i] = new_item(items_list[i], ""); // no descr;
        if (items[i] == NULL) {
            break;
        }
    }
    items[n_items] = (ITEM *)NULL;
    return(items);
}

WINDOW *construct_menu_win(MENU *menu, int nlines, int ncols, int begin_y, int begin_x){
    WINDOW *menu_win = newwin(nlines, ncols, begin_y, begin_x);
    keypad(menu_win, TRUE);
    set_menu_win(menu, menu_win);
    set_menu_format(menu, nlines-2, 1);
    set_menu_sub(menu, derwin(menu_win, nlines-2, ncols-4, 1, 1));
    set_menu_mark(menu, " * ");
    box(menu_win, 0, 0);
    refresh();
    post_menu(menu);
    wrefresh(menu_win);
    return(menu_win);
}

void tui_setup() {
    initscr();
    keypad(stdscr, 1);
    nodelay(stdscr, 1); /* important, for nonblocking getch() */
    cbreak();
    noecho();

    getmaxyx(stdscr, term_height, term_width);

    /* name of program + help */
    w_title = newwin(W_TITLE_H, W_TITLE_W, 0, 0);
    refresh();
    box(w_title, 0, 0);
    wrefresh(w_title);

    /* stmbl connection status window */
    w_status = newwin(W_CON_STATUS_H, W_TITLE_W, W_TITLE_H, 0);
    refresh();
    box(w_status, 0, 0);
    wrefresh(w_status);

    /* shell for user input */
    w_shell = newwin(W_SHELL_H, W_TITLE_W, W_TITLE_H+W_CON_STATUS_H, 0);
    refresh();
    box(w_shell, 0, 0);
    wrefresh(w_shell);

    /* hal categories menu */
    hal_pins_items = construct_menu_items(hal_pins_list, n_hal_pins);
    hal_pins_menu = new_menu((ITEM **)hal_pins_items);
    w_pins = construct_menu_win(hal_pins_menu,
            term_height-W_TITLE_H-W_CON_STATUS_H-W_SHELL_H, W_TITLE_W,
            W_TITLE_H+W_CON_STATUS_H+W_SHELL_H, 0);

    /* stmbl connection output window, also accesed in connection.c */
    w_receive = newwin(term_height, W_CON_RECEIVE_W, 0, W_TITLE_W+1);
    refresh();
    box(w_receive, 0, 0);
    mvwprintw(w_receive, 0, 3, "F5-stop F6-start F7-clear F8-quit");
    wrefresh(w_receive);
    wresize(w_receive, term_height-2, W_CON_RECEIVE_W-2);
    mvwin(w_receive, 1,W_TITLE_W+2);
    scrollok(w_receive, TRUE);
    wclear(w_receive);
    wrefresh(w_receive);
}


void draw_shell() {
    mvwprintw(w_title, 0, 12, "NC-SERVOTERM");
    mvwprintw(w_title, 1, 1, "SHELL: letters-input, enter-send");
    mvwprintw(w_title, 2, 1, "back-d, up/left-hist, down/right-pins");
    mvwprintw(w_shell, 1, 1, ">"); /* input is active */
    mvwprintw(w_shell, 1, 3, shell_buffer);
    wnoutrefresh(w_title);
    wnoutrefresh(w_shell);
}

void draw_hist() {
    mvwprintw(w_title, 0, 12, "NC-SERVOTERM");
    mvwprintw(w_title, 1, 1, "HISTORY: up/down-browse");
    mvwprintw(w_title, 2, 1, "enter/right-return2shell");
    mvwprintw(w_shell, 1, 35, "%d", -hist_i-1);
    mvwprintw(w_shell, 1, 1, "^"); /* history is active */
    mvwprintw(w_shell, 1, 3, shell_buffer);
    wnoutrefresh(w_title);
    wnoutrefresh(w_shell);
}

void draw_pin() {
    mvwprintw(w_title, 0, 12, "NC-SERVOTERM");
    mvwprintw(w_title, 1, 1, "PINS: (pg)up/(pg)dwn-browse");
    mvwprintw(w_title, 2, 1, "enter-add&go2shell, left-go2shell");
    mvwprintw(w_shell, 1, 1, "."); /* pins are active */
    mvwprintw(w_shell, 1, 3, shell_buffer);
    wnoutrefresh(w_title);
    wnoutrefresh(w_shell);
    wnoutrefresh(w_pins);
}

void draw_exit() {
    mvwprintw(w_title, 1, 1, "EXIT: exiting");
    mvwprintw(w_title, 2, 1, "press any key to quit.");
    wnoutrefresh(w_shell);
    wnoutrefresh(w_title);
}

void draw_resize() {
    tui_cleanup();
    tui_setup();
    tui_state = TUI_SHELL;
}

void draw_con(char* description) {
    werase(w_status);
    mvwprintw(w_status, 1, 1, "STMBL is %s.", description);
    box(w_status, 0, 0);
    wnoutrefresh(w_status);
}

void draw_screen() {
    switch(con_state) {
       case CON_DETACHED:   draw_con("detached");   break;
       case CON_STARTING:   draw_con("starting");   break;
       case CON_CONNECTED:  draw_con("connected");  break;
       case CON_ERROR:      draw_con("in error");   break;
    }
    wnoutrefresh(w_receive); /* don't erase this win - it holds buffer */

    werase(w_title);
    werase(w_shell);
    box(w_title, 0, 0);
    box(w_shell, 0, 0);
    switch(tui_state) {
        case TUI_SHELL:     draw_shell();   break;
        case TUI_PIN:       draw_pin();     break;
        case TUI_HIST:      draw_hist();    break;
        case TUI_EXIT:      draw_exit();    break;
        case TUI_RESIZE:    draw_resize();  break;
    }

    doupdate();
}


void shell_clear() {
    memset(shell_buffer, 0, SHELL_BUF_SIZE);
    shell_position = 0;
}

void shell_delete() {
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
}

void shell_send() {
    hist_add();
    shell_send_flag = 1; /* reset from connection.c when sent */
}

void shell_to_hist() {
    memset(shell_buffer, 0, SHELL_BUF_SIZE);
    strncpy(shell_buffer, hist[hist_i], SHELL_BUF_SIZE);
    shell_position = strlen(shell_buffer);
    tui_state = TUI_HIST;
}

void input_shell(int key) {
    switch (key) {
        case 127/*bspace*/: shell_delete();         break;
        case KEY_BACKSPACE: shell_delete();         break;
        case KEY_UP:        shell_to_hist();        break;
        case KEY_LEFT:      shell_to_hist();        break; /* TODO edit inside string */
        case KEY_DOWN:      tui_state = TUI_PIN;    break;
        case KEY_RIGHT:     tui_state = TUI_PIN;    break; /* TODO edit inside string */
        case 10 /*enter*/:  shell_send();           break;
        default:            shell_write(key);       break;
    }
}


void hist_add() { /* copy shell_buffer to first and shift others*/
    int i;
    for (i=SHELL_HIST_SIZE-1; i>0; i--) {
        memset(hist[i], 0, SHELL_BUF_SIZE);
        strncpy(hist[i], hist[i-1], SHELL_BUF_SIZE);
    }
    memset(hist[0], 0, SHELL_BUF_SIZE);
    strncpy(hist[0], shell_buffer, SHELL_BUF_SIZE);
}

void hist_older() {
    /* scroll history up unless at end */
    if (hist_i < SHELL_HIST_SIZE) {
        hist_i++;
        memset(shell_buffer, 0, SHELL_BUF_SIZE);
        strncpy(shell_buffer, hist[hist_i], SHELL_BUF_SIZE);
        shell_position = strlen(shell_buffer);
    }
}

void hist_younger() {
    /* scroll history down unless at beginning */
    if (hist_i > 0) {
        hist_i--;
        mvwprintw(w_shell, 1, 37, "%d", -hist_i);
        memset(shell_buffer, 0, SHELL_BUF_SIZE);
        strncpy(shell_buffer, hist[hist_i], SHELL_BUF_SIZE);
        shell_position = strlen(shell_buffer);
    }
    else {
        tui_state = TUI_SHELL;
        memset(shell_buffer, 0, SHELL_BUF_SIZE);
        shell_position = 0;
    }
}

void hist_to_clean_shell() {
    hist_i = 0;
    memset(shell_buffer, 0, SHELL_BUF_SIZE);
    shell_position = 0;
    tui_state = TUI_SHELL;
}

void hist_to_shell() {
    hist_i = 0;
    tui_state = TUI_SHELL;
}

void input_hist(int key) {
    switch (key) {
        case KEY_UP:        hist_older();           break;
        case KEY_DOWN:      hist_younger();         break;
        case KEY_RIGHT:     hist_to_clean_shell();  break;
        case 10 /*enter*/:  hist_to_shell();        break;
        default:                                    break;
    }
}


void pin_lower() {
    menu_driver(hal_pins_menu, REQ_DOWN_ITEM);
}

void pin_higher() {
    int i = item_index(current_item(hal_pins_menu));
    if (i != 0) menu_driver(hal_pins_menu, REQ_UP_ITEM);
    else tui_state = TUI_SHELL;
}

void pin_pgdown() {
    menu_driver(hal_pins_menu, REQ_SCR_DPAGE);
}

void pin_pgup() {
    menu_driver(hal_pins_menu, REQ_SCR_UPAGE);
}

void pin_to_shell() {
    ITEM *cur_item; 
    int name_length;
    int space_left;

    cur_item = current_item(hal_pins_menu);
    name_length = strlen(item_name(cur_item));
    if (name_length < SHELL_BUF_SIZE) {
        space_left = SHELL_BUF_SIZE - shell_position;
        if (space_left >= name_length) {
            strcat(shell_buffer, item_name(cur_item));
            shell_position += name_length;
        }
    }
    tui_state = TUI_SHELL;
}

void pin_to_clean_shell() {
    tui_state = TUI_SHELL;
}

void input_pin(int key) {
    switch (key) {
        case KEY_DOWN:      pin_lower();            break;
        case KEY_UP:        pin_higher();           break;
        case KEY_NPAGE:     pin_pgdown();           break;
        case KEY_PPAGE:     pin_pgup();             break;
        case KEY_LEFT:      pin_to_clean_shell();   break;
        case 10 /*enter*/:  pin_to_shell();         break;
        default:                                    break;
    }
}


void stop_hal() {
    memset(shell_buffer, 0, SHELL_BUF_SIZE);
    strncpy(shell_buffer, "net0.enable=0", SHELL_BUF_SIZE);
    shell_position = 13;
    shell_send();
}

void start_hal() {
    memset(shell_buffer, 0, SHELL_BUF_SIZE);
    strncpy(shell_buffer, "net0.enable=1", SHELL_BUF_SIZE);
    shell_position = 13;
    shell_send();
}

void input_handle(int key) {
    switch(key) {
        case KEY_F(5):      stop_hal();             break;
        case KEY_F(6):      start_hal();            break;
        case KEY_F(7):      werase(w_receive);      break;
        case KEY_F(8):      tui_state=TUI_EXIT;     break;
    }
    switch(tui_state) {
        case TUI_SHELL:     input_shell(key);       break;
        case TUI_HIST:      input_hist(key);        break;
        case TUI_PIN:       input_pin(key);         break;
        case TUI_EXIT:                              break;
    }
}


void tui_cleanup() {
    int i;
    unpost_menu(hal_pins_menu);
    free_menu(hal_pins_menu);
    for (i=0; i<n_hal_pins; ++i) {
        free_item(hal_pins_items[i]);
    }

    delwin(w_title);
    delwin(w_receive);
    delwin(w_status);
    delwin(w_shell);
    delwin(w_pins);
    erase();
    refresh();
    endwin();
}




