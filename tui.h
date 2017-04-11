/*
 * file: tui.h
 *
 * Text user interface. Doesn't directly deal with connection to stmbl.
 *
 */

#ifndef tui_h
#define tui_h


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

void input_shell(int key);
void input_cat(int key);
void input_pin(int key);
void input_handle(int key);

void tui_setup();
void tui_cleanup();


#endif /* tui_h */
