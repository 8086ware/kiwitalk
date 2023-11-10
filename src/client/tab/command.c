#include <sockmanip.h>
#include "tab.h"
#include <termmanip.h>
#include "parse_command.h"
#include <string.h>

void process_tab_command(struct Tab*** tabs, int *tab_number, int* tab_amount, char* command) {
	struct Tab* tab = (*tabs)[*tab_number];

	int input_arg_amount = 0;
	char** input_args = parse_command(command, &input_arg_amount, " ");

	tm_win_attrib(tab->window_text, TM_ATTRIB_FG_BRIGHTCYAN, 1);

	if(strcmp(input_args[0], "/help") == 0) {
		tm_win_print(tab->window_text, "COMMANDS:\n\n\t/connect [HOSTNAME] [USERNAME] - Will ask for a server hostname to connect to\n\t/exit - Exit kiwitalk\n\t/list - List users in server\n");
	}

	else if(strcmp(input_args[0], "/exit") == 0) {
		for(int i = 0; i < *tab_amount; i++) {
			tm_win_free((*tabs)[i]->window_text);
			free((*tabs)[i]);
		}

		free((*tabs));

		tm_exit();
		exit(0);
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

		else if(strcmp(input_args[1], "list") == 0) {
			tm_win_print(tab->window_text, "%d tabs in current session\n", *tab_amount);
		}
	}

	else if(strcmp(input_args[0], "/connect") == 0) {
		if(input_arg_amount < 3) {
			tm_win_print(tab->window_text, "No username or IP entered\n");
		}

		else {
			tm_win_print(tab->window_text, "Attempting to connect...\n");
			tm_win_update(tab->window_text);

			if(tab->server != NULL) {
				sm_server_free(tab->server);
				tab->server = NULL;
			}

			tab->server = sm_server(input_args[1], KIWITALK_PORT, SM_SERVER_CONNECT, SM_TCP, SM_IPV4);

			if(tab->server == NULL) {
				tm_win_print(tab->window_text, "Couldn't connect to server %s\n", input_args[1]);
			}

			else {
				sm_send(sm_get_server_socket(tab->server), input_args[2], 256, -1);
				tm_win_print(tab->window_text, "Successfully connected to server %s\n", input_args[1]);
				tab->connected = 1;
			}
		}
	}

	else if(strcmp(input_args[0], "/list") == 0) {
		if(tab->connected) {
			sm_send(sm_get_server_socket(tab->server), "LIST", 4, 0);
		}
	}

	free(input_args);
	tm_win_attrib(tab->window_text, TM_ATTRIB_FG_BRIGHTCYAN, 0);
}

