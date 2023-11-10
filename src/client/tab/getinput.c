#include "tab.h"
#include <sockmanip.h>
#include <termmanip.h>
#include <poll.h>
#include <stdio.h>

void get_tab_input(struct Tab*** tabs, int tab_number, int tab_amount, char* command, int max_size) {
	struct Tab* tab = (*tabs)[tab_number];

	struct pollfd s_poll[2];
	s_poll[0].events = POLLIN;
	s_poll[0].fd = fileno(stdin);

	s_poll[1].events = POLLIN;

	if(tab->server != NULL) {
		s_poll[1].fd = tab->server->socket;
		poll(s_poll, 2, -1);
	}

	else {
		poll(s_poll, 1, -1);
	}

	if(s_poll[0].revents & POLLIN) {
		Tm_input c = tm_win_input(tab->window_input);

		if(c.ctrl_character == TM_KEY_CR || c.ctrl_character == TM_KEY_LF) {
			command[tab->input_amount] = '\0';
			tab->finished_input = 1;
			tab->input_amount = 0;
		}

		else if(c.key == TM_KEY_BS || c.key == TM_KEY_DEL) {
			if(tab->input_amount > 0) {
				tab->input_amount--;

				command[tab->input_amount] = '\0';

				int temp_x = tm_win_get_cursor_x(tab->window_input);
				int temp_y = tm_win_get_cursor_y(tab->window_input);
				tm_win_cursor(tab->window_input, temp_x - 1, temp_y);
				tm_win_print(tab->window_input, " ");
				tm_win_cursor(tab->window_input, temp_x - 1, temp_y);
			}
		}

		else if(c.key > 0 && c.key < 128 && tab->input_amount < max_size) {
			tm_win_print(tab->window_input, "%c", c.key);
			command[tab->input_amount] = c.key;
			tab->input_amount++;
		}

		else if(c.terminal_resized) {
			int scr_cols, scr_rows;
			tm_get_scrsize(&scr_cols, &scr_rows);

			for(int i = 0; i < tab_amount; i++) { 
				tm_win_modify((*tabs)[i]->window_text, 0, 0, scr_cols, scr_rows);
				tm_win_modify((*tabs)[i]->window_input, 0, scr_rows - 1, scr_cols, 1);
				tm_win_print((*tabs)[i]->window_input, "> ");
			}
		}
	}
}

