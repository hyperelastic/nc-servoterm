
#include "connection.h"
#include "global.h"

#include <ncurses.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <string.h>
#include <libserialport.h>

struct sp_port *port;
struct sp_port **ports;
pthread_t threads[2];           /* connection: [0]=manage & send, [1]=receive */
char rx = 0;
char message[] = "conf0.p\n"; 
WINDOW *w_con_receive;          /* reserved for the receiver thread */
WINDOW *w_con_status;           /* reserved for the con_manager thread */
int nc_state;
int con_state;


void con_status_print(char* con_status) {
    wmove(w_con_status, 1, 1);
    wclrtoeol(w_con_status);
    wprintw(w_con_status, "STMBL is: %s.", con_status);
    box(w_con_status, 0, 0);
    wrefresh(w_con_status);
}

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

void *con_reciever(void *_) {
    enum sp_return error;
//    char rx = 0;  
    while((nc_state != NC_EXIT) && (con_state != CON_ERROR)) {  
        sp_nonblocking_read(port, &rx, sizeof(rx));
        if isprint(rx) {  
            waddch(w_con_receive, rx);
        }
        else if (rx==10) { //\n
            waddch(w_con_receive, rx);
        }
        rx = 0;

        error = sp_input_waiting(port);
        if (error < 0) {
            con_state = CON_ERROR;
        }

        usleep(1e2);
    }  
    pthread_exit(NULL);
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
        pthread_create(&threads[1], NULL, con_reciever, NULL);
    }
    else {
        con_state = CON_ERROR;
    }
}

void con_write() {
    sp_nonblocking_write(port, &message, sizeof(message));  
}

void *con_manager(void *_) {
    while( nc_state != NC_EXIT) {
        switch(con_state) {
            case CON_DETACHED:
                con_status_print("detached");
                con_port_ping();
                break;
            case CON_STARTING:
                con_status_print("connecting");
                con_init(); /* also starts the reciever thread */
                break;
            case CON_CONNECTED:
                con_status_print("connected");
                wrefresh(w_con_receive);
                con_write();
                break;
            case CON_ERROR:
                pthread_join(threads[1], NULL);
                sp_close(port);
                con_state = CON_DETACHED;
                break;
        }
        usleep(1e6);
    }
    pthread_exit(NULL);
}
