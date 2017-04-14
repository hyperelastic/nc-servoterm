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
WINDOW *w_con_status;
WINDOW *w_con_receive;
WINDOW *w_pins;

/* global connection+display variables */
char shell_buffer[SHELL_BUF_SIZE] = "";
int shell_position = 0;
int shell_send_flag = 0;

/* local */
int hist_i = 0;
char hist[SHELL_HIST_SIZE][SHELL_BUF_SIZE];


void draw_shell() {
    mvwprintw(w_title, 1, 1, "NC-SERVOTERM |shell| input, enter:snd");
    mvwprintw(w_title, 2, 1, "back:d, up/left:hist, down/right:pins");
    mvwprintw(w_shell, 1, 1, ">"); /* input is active */
    mvwprintw(w_shell, 1, 3, shell_buffer);
    box(w_title, 0, 0);
    box(w_shell, 0, 0);
    wnoutrefresh(w_title);
    wnoutrefresh(w_shell);
}

void draw_hist() {
    mvwprintw(w_title, 1, 1, "NC-SERVOTERM |history| up/down:browse");
    mvwprintw(w_title, 2, 1, "enter/right:return 2 shell");
    box(w_title, 0, 0);
    box(w_shell, 0, 0);
    mvwprintw(w_shell, 1, 35, "%d", -hist_i-1);
    mvwprintw(w_shell, 1, 1, "^"); /* history is active */
    mvwprintw(w_shell, 1, 3, shell_buffer);
    wnoutrefresh(w_title);
    wnoutrefresh(w_shell);
}

void draw_pin() {
    mvwprintw(w_title, 1, 1, "NC-SERVOTERM |pins| (pg)up/dwn:browse");
    mvwprintw(w_title, 2, 1, "enter:add&go 2 shell, left:go 2 shell");
    mvwprintw(w_shell, 1, 1, "."); /* pins are active */
    mvwprintw(w_shell, 1, 3, shell_buffer);
    box(w_title, 0, 0);
    box(w_shell, 0, 0);
    wnoutrefresh(w_title);
    wnoutrefresh(w_shell);
    wnoutrefresh(w_pins);
}

void draw_exit() {
    mvwprintw(w_title, 1, 1, "NC-SERVOTERM exit: exiting");
    mvwprintw(w_title, 2, 1, "press any key to quit.");
    box(w_title, 0, 0);
    box(w_shell, 0, 0);
    wnoutrefresh(w_shell);
    wnoutrefresh(w_title);
}

void draw_con(char* description) {
    werase(w_con_status);
    mvwprintw(w_con_status, 1, 1, "STMBL is %s.", description);
    box(w_con_status, 0, 0);
    wnoutrefresh(w_con_status);
}

void draw_screen() {
    werase(w_title);
    werase(w_shell);
    switch(con_state) {
       case CON_DETACHED:   draw_con("detached");   break;
       case CON_STARTING:   draw_con("starting");   break;
       case CON_CONNECTED:  draw_con("connected");  break;
       case CON_ERROR:      draw_con("in error");   break;
    }
    //box(w_title, 0, 0);
    //box(w_con_status, 0, 0);
    wnoutrefresh(w_con_receive); /* don't erase this win - it holds buffer */

    switch(tui_state) {
        case TUI_SHELL:     draw_shell();   break;
        case TUI_PIN:       draw_pin();     break;
        case TUI_HIST:      draw_hist();    break;
        case TUI_EXIT:      draw_exit();    break;
    }

    doupdate();
}

void shell_stop_hal() {
    memset(shell_buffer, 0, SHELL_BUF_SIZE);
    strncpy(shell_buffer, "net0.enable=0", SHELL_BUF_SIZE);
    shell_position = 13;
    shell_send_flag = 1;
}

void shell_start_hal() {
    memset(shell_buffer, 0, SHELL_BUF_SIZE);
    strncpy(shell_buffer, "net0.enable=1", SHELL_BUF_SIZE);
    shell_position = 13;
    shell_send_flag = 1;
}

void shell_clear() {
    memset(shell_buffer, 0, SHELL_BUF_SIZE);
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
}

void shell_send() {
    hist_add();
    hist_i = 0;
    shell_send_flag = 1; /* reset from connection.c when sent */
}

void hist_init() {
    int i;
    for (i=0; i<SHELL_HIST_SIZE; i++) {
        memset(hist[i], 0, SHELL_BUF_SIZE);
        strncpy(hist[i], "", SHELL_BUF_SIZE);
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

void hist_up() {
    /* scroll history up unless at end */
    if (hist_i < SHELL_HIST_SIZE) {
        hist_i++;
        memset(shell_buffer, 0, SHELL_BUF_SIZE);
        strncpy(shell_buffer, hist[hist_i], SHELL_BUF_SIZE);
        shell_position = strlen(shell_buffer);
    }
}

void hist_down() {
    /* scroll history down unless at beginning */
    if (hist_i > 0) {
        hist_i--;
        mvwprintw(w_shell, 1, 37, "%d", -hist_i);
        memset(shell_buffer, 0, SHELL_BUF_SIZE);
        strncpy(shell_buffer, hist[hist_i], SHELL_BUF_SIZE);
        shell_position = strlen(shell_buffer);
    }
    else tui_state = TUI_SHELL;
}

void pin_down() {
    menu_driver(hal_pins_menu, REQ_DOWN_ITEM);
}

void pin_up() {
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

void pin_enter() {
    ITEM *cur_item; 

    cur_item = current_item(hal_pins_menu);
    strcat(shell_buffer, item_name(cur_item));
    shell_position += strlen(item_name(cur_item));                                                

    tui_state = TUI_SHELL;
}

void input_shell(int key) {
    switch (key) {
        case KEY_F(5):      shell_stop_hal();       break;
        case KEY_F(6):      shell_start_hal();      break;
        case KEY_F(7):      shell_clear();          break;
        case KEY_F(8):      tui_state=TUI_EXIT;     break;
        case KEY_BACKSPACE: shell_back();           break;
        case KEY_DOWN:      tui_state=TUI_PIN;      break;
        case KEY_UP:        tui_state=TUI_HIST;     break;
        case KEY_LEFT:      tui_state=TUI_HIST;     break; /* TODO edit inside string */
        case KEY_RIGHT:     tui_state=TUI_PIN;      break; /* TODO edit inside string */
        case 10 /*enter*/:  shell_send();           break;
        default:            shell_write(key);       break;
    }
}

void input_hist(int key) {
    switch (key) {
        case KEY_F(5):      shell_stop_hal();       break;
        case KEY_F(6):      shell_start_hal();      break;
        case KEY_F(7):      shell_clear();          break;
        case KEY_F(8):      tui_state=TUI_EXIT;     break;
        case KEY_DOWN:      hist_down();            break;
        case KEY_UP:        hist_up();              break;
        case KEY_RIGHT:     tui_state=TUI_SHELL;    break;
        case 10 /*enter*/:  tui_state=TUI_SHELL;    break;
        default:                                    break;
    }
}

void input_pin(int key) {
    switch (key) {
        case KEY_F(5):      shell_stop_hal();           break;
        case KEY_F(6):      shell_start_hal();          break;
        case KEY_F(7):      shell_clear();              break;
        case KEY_F(8):      tui_state=TUI_EXIT;         break;
        case KEY_DOWN:      pin_down();                 break;
        case KEY_UP:        pin_up();                   break;
        case KEY_LEFT:      tui_state=TUI_SHELL;        break;
        case KEY_NPAGE:     pin_pgdown();               break;
        case KEY_PPAGE:     pin_pgup();                 break;
        case 10 /*enter*/:  pin_enter();                break;
        default:                                        break;
    }
}

void input_handle(int key) {
    switch(tui_state) {
        case TUI_SHELL:     input_shell(key);       break;
        case TUI_HIST:      input_hist(key);        break;
        case TUI_PIN:       input_pin(key);         break;
        case TUI_EXIT:                              break;
    }
}

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

WINDOW *construct_menu_win(MENU *menu, int nlines, int ncols, int begin_y,
                            int begin_x){
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

    /* name of program + help */
    w_title = newwin(4, 39, 0, 0);
    refresh();
    box(w_title, 0, 0);
    wrefresh(w_title);

    /* stmbl connection status window */
    w_con_status = newwin(3, 39, 4, 0);
    refresh();
    box(w_con_status, 0, 0);
    wrefresh(w_con_status);

    /* shell for user input */
    w_shell = newwin(3, 39, 7, 0);
    refresh();
    box(w_shell, 0, 0);
    wrefresh(w_shell);

    /* hal categories menu */
    hal_pins_items = construct_menu_items(hal_pins_list, n_hal_pins);
    hal_pins_menu = new_menu((ITEM **)hal_pins_items);
    w_pins = construct_menu_win(hal_pins_menu, 14, 39, 10, 0);

    /* stmbl connection output window, also accesed in connection.c */
    w_con_receive = newwin(24, 40, 0, 40);
    refresh();
    box(w_con_receive, 0, 0);
    mvwprintw(w_con_receive, 0, 3, "F5-stop F6-start F7-clear F8-quit");
    wrefresh(w_con_receive);
    mvwin(w_con_receive, 1,41);
    wresize(w_con_receive, 22, 38);
    scrollok(w_con_receive, TRUE);
    wclear(w_con_receive);
    wrefresh(w_con_receive);

    hist_init(); /* TODO move somewhere else perhaps */
}

void tui_cleanup() {
    int i;
    unpost_menu(hal_pins_menu);
    free_menu(hal_pins_menu);
    for (i=0; i<n_hal_pins; ++i) {
        free_item(hal_pins_items[i]);
    }

    delwin(w_title);
    delwin(w_con_receive);
    delwin(w_con_status);
    delwin(w_shell);
    delwin(w_pins);
    erase();
    refresh();
    endwin();
}
