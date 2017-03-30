/*
 * Communication with stmbl board takes place in separate threads from main.
 * More specifically in the con_manager and con_receiver threads.
 * con reciever is a fast loop for receiving, parsing and sending to the screen
 * con_manager is a slow loop, mostly for sending data and refreshing screen.
 */

#ifndef connection_h
#define connection_h

void con_status_print(char* con_status);
void con_port_ping(void);
void *con_reciever(void *_);
void con_init();
void con_write();
void *con_manager(void *_);

#endif




