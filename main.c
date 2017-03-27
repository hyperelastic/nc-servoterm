#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#include <libserialport.h>
#include <ctype.h>

/* Program states */
#define ST_EXIT     0
#define ST_DISCO    1
#define ST_CAT      2
#define ST_PIN      3
#define ST_INPUT    4

pthread_t threads[2];
struct sp_port *port;
struct sp_port **ports;
int pstate = ST_DISCO;
char rx = 0;  
WINDOW *term_win;

void discon_draw() {
    mvprintw(10, 1, "Disconnected");
    refresh();
}
void cat_draw() {
    mvprintw(10, 1, "Categories");
    refresh();
}
void pin_draw() {
    mvprintw(10, 1, "Pins");
    refresh();
}
void input_draw() {
    mvprintw(10, 1, "Input");
    refresh();
}
void exit_draw() {
    wclear(stdscr);
    mvprintw(10, 1, "Exiting");
}

static void screen_draw() {
    move(10, 0);
    clrtoeol();
    switch(pstate) {
        case ST_DISCO:  discon_draw();  break;
        case ST_CAT:    cat_draw();     break;
        case ST_PIN:    pin_draw();     break;
        case ST_INPUT:  input_draw();   break;
        case ST_EXIT:   exit_draw();    break;
    }
}

void discon_key(int key) {
    switch (key) {
        case KEY_F(1):
            pstate=ST_EXIT;
            break;
        case KEY_DOWN:
            mvprintw(12, 10, "Down.");
            break;
        case KEY_UP:
            mvprintw(12, 10, "Up.");
            break;
        default:
            pstate = ST_CAT;
            break;
    }
}

void cat_key(int key) {
    switch (key) {
        case KEY_F(1):
            pstate=ST_EXIT;
            break;
        case KEY_DOWN:
            mvprintw(12, 10, "Down.");
            break;
        case KEY_UP:
            mvprintw(12, 10, "Up.");
            break;
        default:
            pstate = ST_PIN;
            break;
    }
}

void pin_key(int key) {
    switch (key) {
        case KEY_F(1):
            pstate=ST_EXIT;
            break;
        case KEY_DOWN:
            mvprintw(12, 10, "Down.");
            break;
        case KEY_UP:
            mvprintw(12, 10, "Up.");
            break;
        default:
            pstate = ST_INPUT;
            break;
    }
}

void input_key(int key) {
    switch (key) {
        case KEY_F(1):
            pstate=ST_EXIT;
            break;
        case KEY_DOWN:
            mvprintw(12, 10, "Down.");
            break;
        case KEY_UP:
            mvprintw(12, 10, "Up.");
            break;
        default:
            pstate = ST_EXIT;
            break;
    }
}

void input_handle() {
    int key = getch();
    move(12, 0);
    clrtoeol();
    mvprintw(12, 0, "Up/Down:");
    switch(pstate) {
        case ST_DISCO:    return discon_key(key);   break;
        case ST_CAT:      return cat_key(key);      break;
        case ST_PIN:      return pin_key(key);      break;
        case ST_INPUT:    return input_key(key);    break;
        case ST_EXIT:                               break;
    }
    refresh();
}

void prep_stmbl_port(struct sp_port *port){
	sp_set_baudrate(port,38400);
	sp_set_bits(port, 8);
	sp_set_stopbits(port, 1);
	sp_set_parity(port, SP_PARITY_NONE);
	sp_set_xon_xoff(port, SP_XONXOFF_DISABLED);
	sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);
}

static void *disco_spotter(void *_) {
    char *descr;
    char *contains;
    enum sp_return error;
    int i;

    while(pstate == ST_DISCO) {
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
                port = ports[i];
                descr = "attached";
            }
            else {
                descr = "not attached";
            }
        }
        move(2, 0);
        clrtoeol();
        mvprintw(2, 1, "STMBL is: %s.", descr);
        refresh();
        usleep(1e5);
    }
    pthread_exit(NULL);
}

static void *con_reciever(void *_) {
    enum sp_return_error;
    char rx = 0;  
    while((pstate != ST_EXIT) && (pstate != ST_DISCO)) {  
        sp_nonblocking_read(port, &rx, sizeof(rx));  
        if isprint(rx)  
        {  
//            addch(rx);
            waddch(term_win, rx);
        }
        else if (rx==10) { //\n
//            addch(10);
            waddch(term_win, rx);
        }
        rx = 0;  
        usleep(1e2);
    }  
    pthread_exit(NULL);
}

static void *manager(void *_) {
    enum sp_return error;
    char message[] = "conf0\n";
    while (pstate != ST_EXIT) { 
        if (pstate == ST_DISCO) {
            pthread_create(&threads[1], NULL, disco_spotter, NULL);
        }
    
        error = sp_open(port, SP_MODE_READ_WRITE);
        if (error == SP_OK) {
            prep_stmbl_port(port);
            pthread_create(&threads[1], NULL, con_reciever, NULL);
            while (pstate != ST_EXIT) {
//                refresh();
                wrefresh(term_win);
                move(2, 0);
                sp_nonblocking_write(port, &message, sizeof(message));  
                usleep(1e6);
            }
            sp_close(port);
        }
        else {
            pstate = ST_DISCO;
        }
    
        sp_close(port);
        pthread_join(threads[1], NULL);
    }
    pthread_exit(NULL);
}

void nc_setup() {
    initscr();
    keypad(stdscr, 1);
    cbreak();
    noecho();
    mvprintw(1, 1, "Press any key to proceed, up/down arrows for fun, F1 for"
            "exit.");

    term_win = newwin(60, 40, 5, 40); //height, width, starty, startx
    scrollok(term_win, TRUE);
    wrefresh(term_win);

    refresh();
}

void nc_cleanup() {
    erase();
    refresh();
    endwin();
}

int main(int argc, char **argv) {
    nc_setup();
    pthread_create(&threads[0], NULL, manager, NULL);

    while (1) {
        screen_draw();
        if (pstate == ST_EXIT) {
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






