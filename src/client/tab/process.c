#include <sockmanip.h>
#include "tab.h"
#include <termmanip.h>
#include <stdio.h>

void process_tab_input(struct Tab*** tabs, int *tab_number, int* tab_amount, char* input, int max_size) {
	struct Tab* tab = (*tabs)[*tab_number];

	if(tab->finished_input) {
		tab->finished_input = 0;

		tm_win_clear(tab->window_input);
		tm_win_print(tab->window_input, "> ");

		if(*input == '/') {
			process_tab_command(tabs, tab_number, tab_amount, input);
		}

		else if(tab->connected) {
			if(*input != '\0') {
				char send_buf[4096];	
				int bytes_to_send = sprintf(send_buf, "MSG\177%s\177", input);

				if(send(tab->server_socket, send_buf, bytes_to_send, 0) == -1) {
					tm_win_attrib(tab->window_text, TM_ATTRIB_FG_RED, 1);
					tm_win_print(tab->window_text, "Failed to connect to server\n");
					tm_win_attrib(tab->window_text, TM_ATTRIB_FG_RED, 0);
					tab->connected = 0;
				}
			}
		}
	}
}
