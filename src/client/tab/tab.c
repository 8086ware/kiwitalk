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

	((*tabs)[*amount])->server = NULL;

	((*tabs)[*amount])->window_text = tm_window(0, 0, scr_cols, scr_rows);
	((*tabs)[*amount])->window_input = tm_window(0, tm_win_get_rows(((*tabs)[*amount])->window_text) - 1, tm_win_get_columns(((*tabs)[*amount])->window_text), 1);

	tm_win_flags(((*tabs)[*amount])->window_text, TM_FLAG_SCROLL, 1);
	tm_win_flags(((*tabs)[*amount])->window_text, TM_FLAG_ECHO, 0);

	tm_win_input_timeout(((*tabs)[*amount])->window_input, 0); 

	tm_win_parent(((*tabs)[*amount])->window_text, ((*tabs)[*amount])->window_input, TM_CHILD_NORMAL);

	tm_win_print(((*tabs)[*amount])->window_text, "-- KIWITALK --\n\nWelcome. Type /help for list of commands.\n");

	((*tabs)[*amount])->connected = 0;
	((*tabs)[*amount])->finished_input = 0;
	((*tabs)[*amount])->input_amount = 0;

	tm_win_print(((*tabs)[*amount])->window_input, "> ");

	(*amount)++;
}
