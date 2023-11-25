#ifndef TAB_H
#define TAB_H

#define KIWITALK_PORT "47831"

#include <termmanip.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

struct Tab {
	int server_socket;

	Tm_window* window;
	Tm_window* window_text;
	Tm_window* window_input;

	int input_amount;
	_Bool finished_input;

	_Bool connected;
}; 

void new_tab(struct Tab*** tabs, int* amount);
void get_tab_input(struct Tab*** tabs, int tab_number, int amount, char* command, int max_size);
void process_tab_input(struct Tab*** tabs, int *tab_number, int *tab_amount, char* input, int max_size);
void process_tab_command(struct Tab*** tabs, int *tab_number, int* tab_amount, char* command);
void receive_tab_server_requests(struct Tab*** tabs, int* tab_selected, int* amount);

#endif
