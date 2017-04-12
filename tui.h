/*
 * file: tui.h
 *
 * Text user interface. Doesn't directly deal with connection to stmbl.
 *
 */

#ifndef tui_h
#define tui_h

#include <menu.h>


ITEM **hal_categories;
MENU *hal_categories_menu;

void draw_shell(); 
void draw_cat();
void draw_pin();
void draw_exit();
void draw_con(char* description);
void draw_screen();

void shell_set_fault();
void shell_reset_fault();
void shell_clear();
void shell_back();
void shell_write(int input_key);
void shell_send();

void menu_down(MENU *menu);
void menu_up(MENU *menu);

void input_shell(int key);
void input_cat(int key);
void input_pin(int key);
void input_handle(int key);

ITEM **construct_menu_items(char *items_list[], int n_items);
WINDOW *construct_menu_win(MENU *menu, char* name, int nlines, int ncols, int
                            begin_y, int begin_x);
void destruct_menu(MENU *menu, ITEM **items, int n_items);

void tui_setup();
void tui_cleanup();


#endif /* tui_h */
