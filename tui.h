/*
 * file: tui.h
 *
 * Text user interface.
 *
 * Avoid dealing with connection here.
 * Use shell_send_flag to let connection.c know message is ready.
 *
 */

#ifndef tui_h
#define tui_h

#include <menu.h>
#include "global.h"


ITEM **hal_pins_items;
MENU *hal_pins_menu;
int hist_i;
char hist[SHELL_HIST_SIZE][SHELL_BUF_SIZE];
int term_height, term_width;

ITEM **construct_menu_items(char *items_list[], int n_items);
WINDOW *construct_menu_win(MENU *menu, int nlines, int ncols, int begin_y,
        int begin_x);
void tui_setup();

void draw_shell(); 
void draw_hist(); 
void draw_pin();
void draw_exit();
void draw_resize();
void draw_con(char* description);
void draw_screen();

void stop_hal();
void start_hal();
void clear_shell();
void clear_receive();

void shell_clear();
void shell_delete();
void shell_write(int input_key);
void shell_send();
void shell_to_hist();
void input_shell(int key);

void hist_add();
void hist_older();
void hist_younger();
void hist_to_clean_shell();
void hist_to_shell();
void input_cat(int key);

void pin_lower();
void pin_higher();
void pin_pgdown();
void pin_pgup();
void pin_to_shell();
void pin_to_clean_shell();
void input_pin(int key);

void input_handle(int key);

void tui_cleanup();


#endif /* tui_h */
