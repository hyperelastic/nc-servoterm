/*
 * file: connection.h
 *
 * Connection functions. Avoid dealing with ncurses here. One exception is
 * the continous writing to c_con_receive window.
 * 
 */


#ifndef connection_h
#define connection_h


void con_port_ping(void);                   /* checks if STMBL is reachable */
void con_recieve();                         /* reads+parses from STMBL */
void con_init();                            /* sets port */
void con_write();                           /* sends global message to STMBL */
void con_handle();                          /* manages connection */


#endif /* connection_h */




