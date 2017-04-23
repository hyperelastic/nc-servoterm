//#include <stdlib.h>
#include <string.h>
//#include <errno.h>
#include <unistd.h>
//#include <ncurses.h>
//#include <menu.h>
//#include <pthread.h>
//#include <libserialport.h>
//#include <ctype.h>
#include <signal.h>


#include "global.h"
#include "connection.h"
#include "tui.h"

void set_resize() { /* handle terminal resize */
    tui_state = TUI_RESIZE;
}

void hist_init() {
    int i;
    for (i=0; i<SHELL_HIST_SIZE; i++) {
        memset(hist[i], 0, SHELL_BUF_SIZE);
        strncpy(hist[i], "", SHELL_BUF_SIZE);
    }
}

int main() {

    tui_state = TUI_SHELL;
    con_state = CON_DETACHED;

    tui_setup();
    draw_screen();
    hist_init();

    int key = 0;
    int i = 0;
    while (tui_state != TUI_EXIT) {
        con_handle();

        key = getch();      /* set to non blocking in tui/tui_setup() */
        signal(SIGWINCH, set_resize);
        input_handle(key);

        if (i==2e2) {       /* ~50Hz when connected, 0,5 when not */
            draw_screen();
            i = 0;
        }

        i++;
        switch(con_state) {
            case CON_CONNECTED: /* ~10kHz, stmbl parse */
                usleep(1e2);
                break; 
            default:            /* easy on CPU when connecting*/
                usleep(2e4);
                i=2e2;
                break;
        }
    }

    draw_screen();
    nodelay(stdscr, 0); /* for blocking getch() on exit */
    getch();

    tui_cleanup();
}














