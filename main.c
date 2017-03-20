#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/time.h>
#include <locale.h>
#include <ncurses.h>


int pstate = 0;
int read_only = 0;
long update_delay = 100;
static long lastupdate = 999;


static void screen_draw() {
  switch(pstate) {
    case ST_CAT:    cat_draw();     break;
    case ST_PIN:    pin_draw();     break;
    case ST_HELP:   help_draw();    break;
  }
}

void cat_draw() {
    printf("cat");
}

void pin_draw() {
    printf("pin");
}

void help_draw() {
    printf("pin");
}

void calc_init(char *dir, struct dir *org) {
  pstate = ST_CALC;
}

/* wait:
 *  -1: non-blocking, always draw screen
 *   0: blocking wait for input and always draw screen
 *   1: non-blocking, draw screen only if a configured delay has passed or after keypress
 */
int input_handle(int wait) {
  int ch;
  /* ncurses
   * int nodelay(WINDOW *win, bool bf);
   * The nodelay option causes getch to be a non-blocking call. If no input is
   * ready, getch returns ERR. If disabled (bf is FALSE), getch waits until a
   * key is pressed. 
   */
  nodelay(stdscr, wait?1:0); // 1, if wait is true, 0 if not
  if(wait != 1)
    screen_draw();
  else {

    if check waiting; //!!!!!!!!!!!!!!!!!!!!!
      screen_draw();
    }
  }
  while((ch = getch()) != ERR) {
    switch(pstate) {
      case ST_CALC:   return calc_key(ch);
      case ST_BROWSE: return browse_key(ch);
      case ST_HELP:   return help_key(ch);
      case ST_DEL:    return delete_key(ch);
    }
    screen_draw();
  }
  return 0;
}


/* parse command line */
static char *argv_parse(int argc, char **argv) {
  int i, j, len;
  char *dir = NULL;

  /* read from commandline */
  return dir;
}


/* main program */
int main(int argc, char **argv) {
  char *dir;

  if((dir = argv_parse(argc, argv)) == NULL)
    dir = ".";

  calc_init(dir, NULL);

  initscr();
  cbreak();
  noecho();
  /* invisible cursor */
  curs_set(0);
  keypad(stdscr, TRUE);

  while(1) {
    if(pstate == ST_CALC && calc_process())
      break;
    else if(pstate == ST_DEL)
      delete_process();
    else if(input_handle(0))
      break;
  }

  erase();
  refresh();
  endwin();

  return 0;
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






