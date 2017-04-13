//#include <stdlib.h>
//#include <string.h>
//#include <errno.h>
#include <unistd.h>
//#include <ncurses.h>
//#include <menu.h>
//#include <pthread.h>
//#include <libserialport.h>
//#include <ctype.h>


#include "global.h"
#include "connection.h"
#include "tui.h"


int main() {

    tui_state = TUI_SHELL;
    con_state = CON_DETACHED;

    tui_setup();
    draw_screen();

    int key = 0;
    int i = 0;
    while (tui_state != TUI_EXIT) {
        con_handle();

        key = getch();              /* set to non blocking in tui/tui_setup() */
        input_handle(key);

        if (i==2e2) {               /* ~50Hz when connected, 0,5 when not */
            draw_screen();
            i = 0;
        }

        i++;
        switch(con_state) {
            case CON_CONNECTED: usleep(1e2);    break; /* ~10kHz, stmbl parse */
            default:            usleep(1e4);    break; /* ~100Hz, easy on CPU */
        }
    }

    draw_screen();
    nodelay(stdscr, 0); /* for blocking getch() on exit */
    getch();

    tui_cleanup();
}














