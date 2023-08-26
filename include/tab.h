#ifndef TAB_H
#define TAB_H

#include <sockmanip.h>
#include <termmanip.h>

struct Tab {
	Sm_server* server;

	Tm_window* window;
	Tm_window* window_text;
	Tm_window* window_input;

	int input_amount;
	_Bool finished_input;

	_Bool connected;
}; 

void new_tab(struct Tab*** tabs, int* amount);
void get_tab_input(struct Tab*** tabs, int tab_number, char* command, int max_size);
void process_tab_input(struct Tab*** tabs, int *tab_number, int *tab_amount, char* command, int max_size);

#endif
