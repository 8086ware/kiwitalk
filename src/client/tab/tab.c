#include "tab.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

void new_tab(struct Tab*** tabs, int* amount) {
	*tabs = realloc(*tabs, sizeof(struct Tab*) * (*amount + 1));

	if(*tabs == NULL) {
		fprintf(stderr, "Error %d", errno);
		abort();
	}

	(*tabs)[*amount] = malloc(sizeof(struct Tab));

	if((*tabs)[*amount] == NULL) {
		fprintf(stderr, "Error %d", errno);
		abort();
	}

	int scr_cols, scr_rows;

	tm_get_scrsize(&scr_cols, &scr_rows);

	struct Tab* tab = (*tabs)[*amount];

	tab->server = NULL;

	tab->window = tm_window(0, 0, scr_cols, scr_rows);

	tm_win_border(tab->window);

	tab->window_text = tm_window(1, 1, scr_cols - 2, scr_rows - 2);
	tab->window_input = tm_window(1, scr_rows - 2, scr_cols - 2, 1);

	tm_win_input_timeout(tab->window_input, 1); 

	tm_win_parent(tab->window, tab->window_text, TM_CHILD_NORMAL);
	tm_win_parent(tab->window, tab->window_input, TM_CHILD_NORMAL);

	tm_win_flags(tab->window_text, TM_FLAG_SCROLL, 1);
	tm_win_flags(tab->window_text, TM_FLAG_ECHO, 0);

	tm_win_flags(tab->window_input, TM_FLAG_MOUSE_INPUT, 1);
	tm_win_flags(tab->window_input, TM_FLAG_ECHO, 0);
	tm_win_flags(tab->window_input, TM_FLAG_SCROLL, 1);

	tm_win_print(tab->window_text, "-- KIWITALK --\n\nWelcome. Type /help for list of commands.\n");

	tab->connected = 0;
	tab->finished_input = 0;
	tab->input_amount = 0;

	tm_win_print(tab->window_input, "> ");

	tm_win_update(tab->window);
	(*amount)++;
}
