#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#include <libserialport.h>
#include <ctype.h>

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


pthread_t threads[2];   /* connection: [0]=manage and send, [1]=receive */
struct sp_port *port;
struct sp_port **ports;
char rx = 0;                    /* read from port */
char message[] = "conf0.p\n";   /* write to port TODO dynamic */

WINDOW *w_title;             
WINDOW *w_con_receive;            /* reserved for the receiver thread */
WINDOW *w_con_status;             /* reserved for the con_manager thread */

int nc_state;
int con_state;

void draw_cat() {
    mvprintw(10, 1, "Categories");
    refresh();
}

void draw_pin() {
    mvprintw(10, 1, "Pins");
    refresh();
}

void draw_input() {
    mvprintw(10, 1, "Input");
    refresh();
}

void draw_exit() {
    wclear(stdscr);
    mvprintw(1, 1, "Finished. Press any key to exit.");
}

static void draw_screen() {
//    move(10, 0);
//    clrtoeol();
    switch(nc_state) {
        case NC_CATEGORY:   draw_cat();     break;
        case NC_PIN:        draw_pin();     break;
        case NC_INPUT:      draw_input();   break;
        case NC_EXIT:       draw_exit();    break;
    }
}

void input_cat(int key) {
    switch (key) {
        case KEY_F(1):  nc_state=NC_EXIT;           break;
        case KEY_DOWN:  mvprintw(12, 10, "Down.");  break;
        case KEY_UP:    mvprintw(12, 10, "Up.");    break;
        default:        nc_state = NC_PIN;          break;
    }
}

void input_pin(int key) {
    switch (key) {
        case KEY_F(1):  nc_state=NC_EXIT;           break;
        case KEY_DOWN:  mvprintw(12, 10, "Down.");  break;
        case KEY_UP:    mvprintw(12, 10, "Up.");    break;
        default:        nc_state = NC_INPUT;        break;
    }
}

void input_key(int key) {
    switch (key) {
        case KEY_F(1):  nc_state=NC_EXIT;           break;
        case KEY_DOWN:  mvprintw(12, 10, "Down.");  break;
        case KEY_UP:    mvprintw(12, 10, "Up.");    break;
        default:        nc_state = NC_EXIT;         break;
    }
}

void input_handle() {
    int key = getch();
//    move(12, 0);
//    clrtoeol();
    mvprintw(12, 0, "Up/Down:");
    switch(nc_state) {
        case NC_CATEGORY:   input_cat(key);     break;
        case NC_PIN:        input_pin(key);     break;
        case NC_INPUT:      input_key(key);     break;
        case NC_EXIT:                           break;
    }
    refresh();
}

static void con_port_ping(void) {
    char *descr;
    char *contains;
    enum sp_return error;
    int i;

    if (ports) {
        sp_free_port_list(ports);
    }

    error = sp_list_ports(&ports);
    if (error == SP_OK) {
        for(i = 0; ports[i]; i++) {
            descr = sp_get_port_description(ports[i]);
            contains = strstr(descr, "STMBL");
            if (contains) {
                break;
            }
        }
        if (contains) {
            descr = "attached";
            port = ports[i];
            con_state = CON_STARTING;
        }
        else {
            descr = "not attached";
            con_state = CON_DETACHED;
        }
    }

    wmove(w_con_status, 1, 1);
    wclrtoeol(w_con_status);
    wprintw(w_con_status, "STMBL is: %s.", descr);
    box(w_con_status, 0, 0);
    wrefresh(w_con_status);
}

static void *con_reciever(void *_) {
    enum sp_return error;
    char rx = 0;  
    while((nc_state != NC_EXIT) && (con_state != CON_ERROR)) {  
        sp_nonblocking_read(port, &rx, sizeof(rx));
        if isprint(rx) {  
            waddch(w_con_receive, rx);
        }
        else if (rx==10) { //\n
            waddch(w_con_receive, rx);
        }
        rx = 0;

        error = sp_input_waiting(port);
        if (error < 0) {
            con_state = CON_ERROR;
        }

        usleep(1e2);
    }  
    pthread_exit(NULL);
}

void con_init() {
    enum sp_return error;

    error = sp_open(port, SP_MODE_READ_WRITE);
    if (error == SP_OK) {
	    sp_set_baudrate(port,38400);
	    sp_set_bits(port, 8);
	    sp_set_stopbits(port, 1);
	    sp_set_parity(port, SP_PARITY_NONE);
	    sp_set_xon_xoff(port, SP_XONXOFF_DISABLED);
	    sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);

        con_state = CON_CONNECTED;
        pthread_create(&threads[1], NULL, con_reciever, NULL);

    }
    else {
        con_state = CON_ERROR;
    }
}

void con_write() {
    sp_nonblocking_write(port, &message, sizeof(message));  
    usleep(1e6);
}

static void *con_manager(void *_) {
    while( nc_state != NC_EXIT) {
        switch(con_state) {
            case CON_DETACHED:
                con_port_ping();
                break;
            case CON_STARTING:
                con_init(); /* also starts the reciever thread */
                break;
            case CON_CONNECTED:
                wrefresh(w_con_receive);
                con_write();
                break;
            case CON_ERROR:
                pthread_join(threads[1], NULL);
                sp_close(port);
                con_state = CON_DETACHED;
                break;
        }
        usleep(1e3);
    }
    pthread_exit(NULL);
}

void nc_setup() {
    initscr();
    keypad(stdscr, 1);
    cbreak();
    noecho();

    w_title = newwin(5, 80, 0, 0);
    box(w_title, 0, 0);
    mvwprintw(w_title, 1, 1, "NC-SERVOTERM");
    mvwprintw(w_title, 3, 1, "Press any key to proceed, up/down arrows for "
            "fun, F1 for exit.");
    refresh();
    wrefresh(w_title);

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

void nc_cleanup() {
    erase();
    refresh();
    endwin();
}

int main(int argc, char **argv) {

    nc_state = NC_CATEGORY;
    con_state = CON_DETACHED;

    nc_setup();
    pthread_create(&threads[0], NULL, con_manager, NULL);

    while (1) {
        draw_screen();
        if (nc_state == NC_EXIT) {
            break;
        }
        else {
            input_handle();
        }
    }

    getch();
    nc_cleanup();
    pthread_exit(NULL);
}








































//#include <stdio.h>
//#include <stdlib.h>
//#include <ncurses.h>
//#include <menu.h>
//#include "hal_list.h"
//#define N_FLAGS 2
//
//void print_flags(WINDOW *flag_win, char *flags[], int highlights[], int size);
//ITEM **construct_menu_items(char *items_list[], int n_items,
//                            void (*userptr_func)(void)); /* pass userptr function*/
//WINDOW *construct_menu_win(MENU *menu, char *name, int nlines, int ncols,
//                            int begin_y, int begin_x);
//void userptr_func0(int item_index);
//
//int main() {
//    WINDOW *flag_win;
//    WINDOW *hal_categories_win;
//    ITEM **hal_categories;
//    MENU *hal_categories_menu;
//    WINDOW *hal_pins_win;
//    ITEM **hal_pins;
//    MENU *hal_pins_menu;
//    ITEM *cur_item; 
//    void (*f)(void); /* userptr placeholder*/
//    int i, i_item;
//    int highlights[N_FLAGS] = {
//        0,
//        0
//    };
//    char *flags[N_FLAGS] = {
//        "Attached",
//        "Connected"
//    };
//    int key;
//    int mode = 0; /*0: categories, 1: pins */
//
//    initscr();
//    cbreak();
//    noecho();
//
//    flag_win = newwin(6, 13, 2, 1);
//    refresh();
//    print_flags(flag_win, flags, highlights, N_FLAGS);
//    mvprintw(0, 0, "Use left/right arrow keys to toggle flags. F2 to exit.");
//
////    hal_categories = construct_menu_items(hal_categories_list,
////                        n_hal_categories, userptr_func0);
//    hal_categories = construct_menu_items(hal_categories_list,
//                        n_hal_categories, NULL);
//    hal_categories_menu = new_menu((ITEM **)hal_categories);
//    hal_categories_win = construct_menu_win(hal_categories_menu,
//                            "hal categories", 20, 20, 9, 1);
//    hal_pins = construct_menu_items(hal_pins_list[0], n_hal_pins[0], NULL);
//    hal_pins_menu = new_menu((ITEM **)hal_pins);
//    hal_pins_win = construct_menu_win(hal_pins_menu, "hal pins", 20, 50, 9, 22);
//
//    while(true) {
//        print_flags(flag_win, flags, highlights, N_FLAGS);
//        if (key == KEY_F(2)) {
//            break;
//        }
//        refresh();
//        switch(mode) {
//            case 0: {
//                key=wgetch(hal_categories_win);
//                switch(key) {
//                    case KEY_RIGHT:
//                        highlights[0] = !highlights[0];
//                        break;
//                    case KEY_LEFT:
//                        highlights[1] = !highlights[1];
//                        break;
//                    case KEY_DOWN:
//                        menu_driver(hal_categories_menu, REQ_DOWN_ITEM);
//                        break;
//                    case KEY_UP:
//                        menu_driver(hal_categories_menu, REQ_UP_ITEM);
//                        break;
//                    case 10: { /* Enter */ 
//                        cur_item = current_item(hal_categories_menu);
//                        i_item = item_index(cur_item);
//                        hal_pins = construct_menu_items(hal_pins_list[i_item],
//                            n_hal_pins[i_item], NULL);
//                        hal_pins_menu = new_menu((ITEM **)hal_pins);
//                        hal_pins_win = construct_menu_win(hal_pins_menu,
//                                                "hal pins", 20, 50, 9, 22);
////                        f = item_userptr(cur_item);
////                        f((int)item_index(cur_item)); /*pass category index*/
////                        pos_menu_cursor(hal_categories_menu);
//                        mode = 1;
//                        break;
//                    }
//                }
//                wrefresh(hal_categories_win);
//                break;
//            }
//            case 1: {
//                key=wgetch(hal_pins_win);
//                switch(key) {
//                    case KEY_RIGHT:
//                        highlights[0] = !highlights[0];
//                        break;
//                    case KEY_LEFT:
//                        highlights[1] = !highlights[1];
//                        break;
//                    case KEY_DOWN:
//                        menu_driver(hal_pins_menu, REQ_DOWN_ITEM);
//                        break;
//                    case KEY_UP:
//                        menu_driver(hal_pins_menu, REQ_UP_ITEM);
//                        break;
//                    case 10: { /* Enter */ 
//                        cur_item = current_item(hal_pins_menu);
//////                        f = item_userptr(cur_item);
//////                        f((int)item_index(cur_item)); /*pass category index*/
//////                        pos_menu_cursor(hal_categories_menu);
//                        mode = 0;
//                        break;
//                    }
//                wrefresh(hal_pins_win);
//                break;
//                }
//            }
//        }
//    }
//
//    unpost_menu(hal_categories_menu);
//    free_menu(hal_categories_menu);
//    for (i=0; i<n_hal_categories; ++i) {
//        free_item(hal_categories[i]);
//    }
//
//    unpost_menu(hal_pins_menu);
//    free_menu(hal_pins_menu);
////    for (i=0; i<n_hal_pins[i_item]; ++i) {
////        free_item(hal_pins[i]);
////    }
//    endwin();
//    return 0;
//}
//
//void userptr_func0(int item_index) { 
//    /* To be performed on enter in mode 0 */
////    move(40, 0);
////	clrtoeol();
////	mvprintw(40, 0, "Item selected is : %d", item_index);
//}	
//
//WINDOW *construct_menu_win(MENU *menu, char* name, int nlines, int ncols, int begin_y, int begin_x){
//    WINDOW *menu_win = newwin(nlines, ncols, begin_y, begin_x);
//    keypad(menu_win, TRUE);
//    set_menu_win(menu, menu_win);
//    set_menu_sub(menu, derwin(menu_win, nlines-4, ncols-4, 3, 1));
//    set_menu_mark(menu, " * ");
//    box(menu_win, 0, 0);
//    mvwprintw(menu_win, 1, 1, name);
//    post_menu(menu);
//    refresh();
//    wrefresh(menu_win);
//    return(menu_win);
//}
//
//ITEM **construct_menu_items(char *items_list[], int n_items, void (*userptr_func)(void)) {
//    /*
//        Pass NULL to leave the userptr functions unasigned
//    */
//    ITEM **items;
//    int i;
//    items = (ITEM **)calloc(n_items, sizeof(ITEM *));
//    for(i=0; i<n_items; ++i) {
//        items[i] = new_item(items_list[i], ""); // no descr;
//        if (items[i] == NULL) {
//            break;
//        }
//        if (userptr_func) {
//		    set_item_userptr(items[i], userptr_func);
//        }
//    }
//    items[n_items] = (ITEM *)NULL;
//    return(items);
//}
//
//void destruct_menu_items(ITEM **items, int n_items) {
//    int i;
//    for (i=0; i<n_items; ++i) {
//        free_item(items[i]);
//    }
//}
//
//void print_flags(WINDOW *flag_win, char *flags[], int highlights[], int size) {
//    /*
//        STMBL attached, connected flags
//    */
//    int x, y, i;    
//
//    box(flag_win, 0, 0);
//    mvwprintw(flag_win, 1, 2, "STMBL");
//    x = 2;
//    y = 3;
//    for(i = 0; i < size; ++i) {
//        if(highlights[i]) {
//            wattron(flag_win, A_REVERSE); 
//            mvwprintw(flag_win, y, x, "%s", flags[i]);
//            wattroff(flag_win, A_REVERSE);
//        }
//        else
//            mvwprintw(flag_win, y, x, "%s", flags[i]);
//        ++y;
//    }
//    wrefresh(flag_win);
//}






