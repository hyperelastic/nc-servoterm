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
WINDOW *w_hal_categories;

/* global connection+display variables */
char shell_buffer[SHELL_BUF_SIZE] = "";
int shell_position = 0;
int shell_send_flag = 0;


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
//    werase(w_con_status);
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
    wnoutrefresh(w_hal_categories);
//    post_menu(hal_categories_menu);
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

void menu_down(MENU *menu) {
    menu_driver(hal_categories_menu, REQ_DOWN_ITEM);
}

void menu_up(MENU *menu) {
    menu_driver(hal_categories_menu, REQ_UP_ITEM);
}

void menu_enter(MENU *menu) {
    ITEM *cur_item; 
    int i;
    char name_cur_item[20];

    memset(shell_buffer, 0, sizeof(shell_buffer));
    cur_item = current_item(menu);
    strncpy(shell_buffer, item_name(cur_item), sizeof(shell_buffer));
    /* TODO TODO TODO TODO TODO TODO TODO */
    /* TODO variable shell position  TODO */
    /* TODO TODO TODO TODO TODO TODO TODO */
    shell_position = 13;                                                
    shell_send_flag = 1;

    tui_state = TUI_SHELL;
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
        case KEY_F(8):  tui_state=TUI_EXIT;                     break;
        case KEY_F(5):  tui_state=TUI_SHELL;                    break;
        case KEY_F(7):  tui_state=TUI_PIN;                      break;
        case KEY_F(9):  shell_set_fault();                      break;
        case KEY_F(10): shell_reset_fault();                    break;
        case KEY_F(11): shell_clear();                          break;
        case KEY_DOWN:  menu_down(hal_categories_menu);         break;
        case KEY_UP:    menu_up(hal_categories_menu);           break;
        case 10 /*enter*/:  menu_enter(hal_categories_menu);    break;
        default:                                                break;
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

void destruct_menu(MENU *menu, ITEM **items, int n_items) {
    unpost_menu(menu);
    free_menu(menu);
    int i;
    for (i=0; i<n_items; ++i) {
        free_item(items[i]);
    }
}

WINDOW *construct_menu_win(MENU *menu, char* name, int nlines, int ncols, int begin_y, int begin_x){
    WINDOW *menu_win = newwin(nlines, ncols, begin_y, begin_x);
    keypad(menu_win, TRUE);
    set_menu_win(menu, menu_win);
    set_menu_sub(menu, derwin(menu_win, nlines-4, ncols-4, 3, 1));
    set_menu_mark(menu, " * ");
    box(menu_win, 0, 0);
    mvwprintw(menu_win, 1, 1, name);
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

    /* hal categories */
    hal_categories = construct_menu_items(hal_categories_list, n_hal_categories);
    hal_categories_menu = new_menu((ITEM **)hal_categories);
    w_hal_categories = construct_menu_win(hal_categories_menu, "HAL categories",
                        19, 39, 11, 0);
}

void tui_cleanup() {
    delwin(w_title);
    delwin(w_con_receive);
    delwin(w_con_status);
    delwin(w_shell);
    delwin(w_hal_categories);
    erase();
    refresh();
    endwin();
}
