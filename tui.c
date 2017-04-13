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


void draw_shell() {
    mvwprintw(w_title, 1, 1, "NC-SERVOTERM shell-write,enter,up,dwn");
    mvwprintw(w_title, 2, 1, "F5-stop F6-start F7-clear F8-quit");
    mvwprintw(w_shell, 1, 1, ">");
    mvwprintw(w_shell, 1, 3, shell_buffer);
}

void draw_pin() {
    mvwprintw(w_title, 1, 1, "NC-SERVOTERM pin-(pg)up,(pg)down,entr");
    mvwprintw(w_title, 2, 1, "F5-stop F6-start F7-clear F8-quit");
}

void draw_exit() {
    mvwprintw(w_title, 1, 1, "NC-SERVOTERM exit");
    mvwprintw(w_title, 2, 1, "Exiting. Press any key to quit.");
}

void draw_con(char* description) {
    mvwprintw(w_con_status, 1, 1, "STMBL is %s.", description);
}

void draw_screen() {
    werase(w_title);
    werase(w_shell);
    switch(tui_state) {
        case TUI_SHELL:     draw_shell();   break;
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
    wnoutrefresh(w_pins);
    doupdate();
}

void shell_set_fault() {
    memset(shell_buffer, 0, sizeof(shell_buffer));
    strncpy(shell_buffer, "net0.enable=0", sizeof(shell_buffer));
    shell_position = 13;
    shell_send_flag = 1;
}

void shell_reset_fault() {
    memset(shell_buffer, 0, sizeof(shell_buffer));
    strncpy(shell_buffer, "net0.enable=1", sizeof(shell_buffer));
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

void shell_down() {
    tui_state = TUI_PIN; /* TODO history, only switch state when reached end */
}

void shell_write(int input_key) {
    if (isprint(input_key)) {
        if (shell_position < SHELL_BUF_SIZE-1) {
            shell_buffer[shell_position] = input_key;
            shell_position++;
            shell_buffer[shell_position] = '\0';
//TODO        shell_buffer[shell_position++] = '\0';
        }
    }
}

void shell_send() {
    shell_send_flag = 1; /* reset from connection.c when sent */
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
    int i;
    char name_cur_item[20];

    memset(shell_buffer, 0, sizeof(shell_buffer));
    cur_item = current_item(hal_pins_menu);
    strncpy(shell_buffer, item_name(cur_item), sizeof(shell_buffer));
    shell_position = strlen(item_name(cur_item));                                                

    tui_state = TUI_SHELL;
}

void input_shell(int key) {
    switch (key) {
        case KEY_F(5):      shell_set_fault();      break;
        case KEY_F(6):      shell_reset_fault();    break;
        case KEY_F(7):      shell_clear();          break;
        case KEY_F(8):      tui_state=TUI_EXIT;     break;
        case KEY_BACKSPACE: shell_back();           break;
        case 10 /*enter*/:  shell_send();           break;
        case KEY_DOWN:      shell_down();           break; /* TODO history*/
//        case KEY_UP:    tui_state=TUI_CATEGORY;     break; /* TODO history*/
        default:            shell_write(key);        break;
    }
}

void input_pin(int key) {
    switch (key) {
        case KEY_F(5):      shell_set_fault();          break;
        case KEY_F(6):      shell_reset_fault();        break;
        case KEY_F(7):      shell_clear();              break;
        case KEY_F(8):      tui_state=TUI_EXIT;         break;
        case KEY_DOWN:      pin_down();                 break;
        case KEY_UP:        pin_up();                   break;
        case KEY_NPAGE:     pin_pgdown();               break;
        case KEY_PPAGE:     pin_pgup();                 break;
        case 10 /*enter*/:  pin_enter(hal_pins_menu);   break;
        default:                                        break;
    }
}

void input_handle(int key) {
    switch(tui_state) {
        case TUI_SHELL:     input_shell(key);       break;
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
    w_pins = construct_menu_win(hal_pins_menu, 19, 39, 10, 0);

    /* stmbl connection output window, also accesed in connection.c */
    w_con_receive = newwin(24, 40, 0, 40);
    refresh();
    box(w_con_receive, 0, 0);
    wrefresh(w_con_receive);
    mvwin(w_con_receive, 1,41);
    wresize(w_con_receive, 22, 38);
    scrollok(w_con_receive, TRUE);
    wclear(w_con_receive);
    wrefresh(w_con_receive);
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
