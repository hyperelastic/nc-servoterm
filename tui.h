/*
 * file: tui.h
 *
 * Text user interface. Runs in main().
 *
 */

#ifndef tui_h
#define tui_h

void draw_cat();
void draw_pin();
void draw_input(); 
void draw_exit();
void draw_screen();

void input_cat(int key);
void input_pin(int key);
void input_key(int key);
void input_handle();

void tui_setup();
void tui_cleanup();


#endif /* tui_h */
