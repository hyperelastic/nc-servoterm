/*
 * file: connection.c
 */


#include <ncurses.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <libserialport.h>

#include "connection.h"
#include "global.h"


/* global program states */
int con_state;

/* global connection + tui shared variables */
int shell_send_flag;    /* set to 1 from tui.c */

/* global ncurses-specific */
WINDOW *w_con_receive;

/* local */
struct sp_port *port;
struct sp_port **ports;
char rx = 0;
int wave_count = -1;


void con_port_ping(void) {
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
    }
    else {
        con_state = CON_ERROR;
    }
}

void con_recieve() {
    enum sp_return error;

    sp_nonblocking_read(port, &rx, sizeof(rx));

    if (rx==-1) wave_count = 0;             /* rx==(char)0xFF, wave announced */

    if (wave_count<0) {
        if (isprint(rx)) {  
            waddch(w_con_receive, rx);
        }
        else if (rx==10) { //\n
            waddch(w_con_receive, rx);
        }
    }
    else if (wave_count<8) wave_count++;    /* stmbl sending wave TODO handle */
    else wave_count = -1;                   /* stmbl stopped sending wave */

    rx = 0;
    error = sp_input_waiting(port);
    if (error < 0) {
        con_state = CON_ERROR;
    }
    
}

void con_write() {
    if (shell_send_flag == 1) {
        char message[shell_position+1];
        strncpy(message, shell_buffer, shell_position+1);
        message[shell_position] = '\n';
        message[shell_position+1] = '\0';
        sp_nonblocking_write(port, message, sizeof(message));  

        /* clear shell buffer */
        memset(shell_buffer, 0, SHELL_BUF_SIZE);
        shell_position = 0;

        shell_send_flag = 0;
    }
}

void con_handle() {
        switch(con_state) {
            case CON_DETACHED:
                con_port_ping();
                break;
            case CON_STARTING:
                con_init();
                break;
            case CON_CONNECTED:
                con_recieve();
                con_write();
                break;
            case CON_ERROR:
                sp_close(port);
                con_state = CON_DETACHED;
                break;
        }
}




