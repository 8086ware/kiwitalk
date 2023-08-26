#include "tab.h"
#include <stdio.h>

#define KIWITALK_PORT "47831"

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
	((*tabs)[*amount])->window = tm_window(0, 0, scr_cols, scr_rows);

	tm_win_flags(((*tabs)[*amount])->window, TM_FLAG_SCROLL, 1);
	tm_win_flags(((*tabs)[*amount])->window, TM_FLAG_INPUTBLOCK | TM_FLAG_ECHO, 0);

	tm_win_border(((*tabs)[*amount])->window);

	((*tabs)[*amount])->window_text = tm_window(1, 1, tm_win_get_columns(((*tabs)[*amount])->window) - 2, tm_win_get_rows(((*tabs)[*amount])->window) - 3);
	((*tabs)[*amount])->window_input = tm_window(1, tm_win_get_rows(((*tabs)[*amount])->window) - 2, tm_win_get_columns(((*tabs)[*amount])->window) - 2, 1);

	tm_win_parent(((*tabs)[*amount])->window,((*tabs)[*amount])->window_text, TM_CHILD_NORMAL);
	tm_win_parent(((*tabs)[*amount])->window, ((*tabs)[*amount])->window_input, TM_CHILD_NORMAL);

	tm_win_print(((*tabs)[*amount])->window_text, "-- KIWITALK --\n\nWelcome. Type /help for list of commands.\n");

	((*tabs)[*amount])->connected = 0;
	((*tabs)[*amount])->finished_input = 0;
	((*tabs)[*amount])->input_amount = 0;

	tm_win_print(((*tabs)[*amount])->window_input, "> ");

	(*amount)++;
}

void get_tab_input(struct Tab*** tabs, int tab_number, char* command, int max_size) {
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

	else if(c != 0 && tab->input_amount < max_size) {
		tm_win_print(tab->window_input,"%c", c);
		tm_win_update(tab->window_input);
		command[tab->input_amount] = c;
		tab->input_amount++;
	}
}

void process_tab_input(struct Tab*** tabs, int *tab_number, int* tab_amount, char* command, int max_size) {
	struct Tab* tab = (*tabs)[*tab_number];

	if(tab->finished_input) {
		tab->finished_input = 0;

		tm_win_clear(tab->window_input);
		tm_win_print(tab->window_input, "> ");

		if(*command == '/') {
			char** input_args = NULL;
			int input_arg_amount = 0;

			char* temp = NULL;

			do {
				if(input_arg_amount == 0) {
					temp = strtok(command, " ");
				}

				else {
					temp = strtok(NULL, " ");
				}

				if(temp != NULL) {
					input_args = realloc(input_args, sizeof(char*) * (input_arg_amount + 1));
					input_args[input_arg_amount] = temp;
					input_arg_amount++;
				}
			} while(temp != NULL);

			if(strcmp(input_args[0], "/help") == 0) {
				tm_win_print(tab->window_text, "COMMANDS:\n\nconnect [HOSTNAME] [USERNAME] - Will ask for a server hostname to connect to\nexit - Exit kiwitalk\n");
			}

			else if(strcmp(input_args[0], "/exit") == 0) {
				tm_win_free(tab->window);
				tm_exit();
			}

			else if(strcmp(input_args[0], "/tab") == 0) {
				if(input_arg_amount < 2) {
						tm_win_print(tab->window_text, "No subcommand given\n");
				}

				else if(strcmp(input_args[1], "select") == 0) {
					if(input_arg_amount < 3) {
						tm_win_print(tab->window_text, "No tab number given\n");
					}

					else {
						*tab_number = atoi(input_args[2]);
					}
				}

				else if(strcmp(input_args[1], "new") == 0) {
					new_tab(tabs, tab_amount);
				}
			}

			else if(strcmp(input_args[0], "/connect") == 0) {
				if(input_arg_amount < 3) {
					tm_win_print(tab->window_text, "No username or IP entered\n");
					tm_win_update(tab->window);
				}

				else {
					tm_win_print(tab->window_text, "Attempting to connect...\n");
					tm_win_update(tab->window);

					tab->server = sm_server(input_args[1], KIWITALK_PORT, SM_SERVER_CONNECT, SM_TCP, SM_IPV4);

					if(tab->server == NULL) {
						tm_win_print(tab->window_text, "Couldn't connect to server %s.\n", input_args[2]);
					}

					else {
						sm_send(sm_get_server_socket(tab->server), input_args[2], 256, -1);
						tab->connected = 1;
					}
				}
			}

			free(input_args);
		}

		else if(tab->connected) {
			if(*command != '\0') {
				sm_send(sm_get_server_socket(tab->server), command, strlen(command) + 1, 0);
			}
		}
	}

	if(tab->connected) {
		char receive_buffer[4096];
		int receive_bytes = sm_receive(sm_get_server_socket(tab->server), receive_buffer, 4096, 0);

		receive_buffer[receive_bytes] = '\0';

		if(receive_bytes > 0) {
			tm_win_attrib(tab->window_text, TM_ATTRIB_FG_BRIGHTGREEN, 1);
			tm_win_print(tab->window_text, receive_buffer);
			tm_win_print(tab->window_text, "\n");
			tm_win_attrib(tab->window_text, TM_ATTRIB_FG_BRIGHTGREEN, 0);
		}
	}
}
