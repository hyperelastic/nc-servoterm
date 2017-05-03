/*
 * file: connection.c
 *
 * Parses stmbl output, sends messages to stmbl..
 *
 * What are waves?
 *
 * Stmbl sends printable messages. In addition it sends eight uint8_t messages,
 * which are announced by a 0xFF message. Those eight numbers are called waves,
 * since they are used in the original servoterm to create plots. They are the
 * values of term0.wave[0-7] hal pins. By default wave[1] is associated with
 * the encoder position.
 *
 * The wave protocol is this:
 * "some text" + 0xFF + uint8_t wave[0] + uint8_t wave[1] + ... +
 * uint8_t wave[7] + "other text"
 *
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
float wave[8] = {0., 0., 0., 0., 0., 0., 0., 0. }; /* term0.wave[0-7], -0.5 to 0.5 */

/* global ncurses-specific */
WINDOW *w_receive;

/* local */
struct sp_port *port;
struct sp_port **ports;
int wave_count = -1;    /* -1 no wave, 0-7 wave  */
uint8_t rx = 0;         /* usb communication buffer */


void con_port_ping(void) {
    char *descr;
    enum sp_return error;
    int i;

    char *contains = NULL;

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
    
        con_state = CON_RECEIVE;
    }
    else {
        con_state = CON_ERROR;
    }
}

void con_recieve() {
    enum sp_return error;

    sp_nonblocking_read(port, &rx, sizeof(rx));

    if (wave_count==-1) {       /* no wave, normal */
        if (isprint(rx)) {  
            waddch(w_receive, rx);
        }
        else if (rx==10) {          /* new line */
            waddch(w_receive, rx);
        }
        else if (rx==0xFF) {        /* wave announced */
            wave_count = 0;
        }
    } 
    else if (wave_count<8) {    /* process wave */
        wave[wave_count] = (rx-128)/128.;
        wave_count++;
    }
    else {                      /* wave over, back to normal */
        wave_count = -1;
    }

    rx = 0;
    error = sp_input_waiting(port);
    if (error < 0) {
        con_state = CON_ERROR;
    }

    if (shell_send_flag == 1) {
        con_state = CON_SEND;
        shell_send_flag = 0;
    }
    
}

void con_write() {
    enum sp_return error;

    char message[shell_position+1];
    strncpy(message, shell_buffer, shell_position+1);
    message[shell_position] = '\n';
    message[shell_position+1] = '\0';
    sp_nonblocking_write(port, message, sizeof(message));  

    /* clear shell buffer */
    memset(shell_buffer, 0, SHELL_BUF_SIZE);
    shell_position = 0;

    error = sp_output_waiting(port);
    if (!(error < 0)) {
        con_state = CON_RECEIVE;
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
            case CON_RECEIVE:
                con_recieve();
                break;
            case CON_SEND:
                con_write();
                break;
            case CON_ERROR:
                sp_close(port);
                con_state = CON_DETACHED;
                break;
        }
}




