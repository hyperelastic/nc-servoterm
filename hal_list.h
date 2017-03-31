/*
 * file hal_list.h
 *
 * Lists of stmbl hal categories and pins for creating
 * the corresponding ncurses menus.
 *
 */


#ifndef HAL_LIST_INCLUDED
#define HAL_LIST_INCLUDED


extern char *hal_categories_list[];
extern int n_hal_categories;
extern char **hal_pins_list[];
extern int n_hal_pins[];
extern void test_hal_categories();
extern void test_hal_pins();


#endif
