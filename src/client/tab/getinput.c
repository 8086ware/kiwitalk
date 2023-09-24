#include "tab.h"
#include <sockmanip.h>
#include <termmanip.h>

void get_tab_input(struct Tab*** tabs, int tab_number, int tab_amount, char* command, int max_size) {
	struct Tab* tab = (*tabs)[tab_number];

	int c = tm_win_input_ch(tab->window_input);

	if(c == '\n' || c == '\r') {
		command[tab->input_amount] = '\0';
		tab->finished_input = 1;
		tab->input_amount = 0;
	}

	else if(c == '\b' || c == '\177') {
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

	else if(c > 0 && c < 128 && tab->input_amount < max_size) {
		tm_win_print(tab->window_input,"%c", c);
		tm_win_update(tab->window_input);
		command[tab->input_amount] = c;
		tab->input_amount++;
	}

	else if(c == TM_KEY_SCREEN_RESIZED) {
		int scr_cols, scr_rows;
		tm_get_scrsize(&scr_cols, &scr_rows);
	
		for(int i = 0; i < tab_amount; i++) { 
			tm_win_modify((*tabs)[i]->window_text, 0, 0, scr_cols, scr_rows - 1);
			tm_win_modify((*tabs)[i]->window_input, 0, scr_rows - 1, scr_cols, 1);
			tm_win_print((*tabs)[i]->window_input, "> ");
		}

		tm_win_update(tab->window_text);
		tm_win_update(tab->window_input);
	}
}
