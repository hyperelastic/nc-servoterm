/*
 * file: connection.h
 * 
 * Communication with stmbl board takes place in separate threads from main.
 * specifically in the con_manager and con_receiver threads.
 *
 * con reciever is a fast loop for receiving, parsing and sending to the screen
 * con_manager is a slow loop, mostly for sending data and refreshing screen.
 */

#ifndef connection_h
#define connection_h

void con_status_print(char* con_status);    /* puts STMBL response in window */
void con_port_ping(void);                   /* checks if STMBL is reachable */
void *con_reciever(void *_);                /* reads+parses from STMBL */
void con_init();                            /* sets port */
void con_write();                           /* sends global message to STMBL */
void *con_manager(void *_);                 /* manages separate from main() */

#endif




