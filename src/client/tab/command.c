#include "tab.h"
#include <termmanip.h>
#include "parse_command.h"
#include <string.h>
#ifndef _WIN32
#include <fcntl.h>
#endif
#include <stdlib.h>

void process_tab_command(struct Tab*** tabs, int *tab_number, int* tab_amount, char* command) {
	struct Tab* tab = (*tabs)[*tab_number];

	int input_arg_amount = 0;
	char** input_args = parse_command(command, &input_arg_amount, " ");

	tm_win_attrib(tab->window_text, TM_ATTRIB_FG_BRIGHTCYAN, 1);

	if(strcmp(input_args[0], "/help") == 0) {
		tm_win_print(tab->window_text, "COMMANDS:\n\n\t/connect [HOSTNAME] [USERNAME] - Will ask for a server hostname to connect to\n\t/exit - Exit kiwitalk\n\t/list - List users in server\n\t/clear - Clears screen\n");
	}

	else if(strcmp(input_args[0], "/exit") == 0) {
		for(int i = 0; i < *tab_amount; i++) {
			tm_win_free((*tabs)[i]->window);
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
				int new_tab_number = atoi(input_args[2]);

				if(new_tab_number > tab_amount - 1) {
					tm_win_print(tab->window_text, "Invalid tab number");
				}

				else {
					*tab_number = new_tab_number;
				}
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

			struct addrinfo hints;
			memset(&hints, 0, sizeof(hints));

			struct addrinfo* servinfo = NULL;

			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;

			int gairesult = getaddrinfo(input_args[1], KIWITALK_PORT, &hints, &servinfo);

			if(gairesult == -1 || servinfo == NULL) {
				tm_win_print(tab->window_text, "Server doesn't exist %s\n", input_args[1]);
			}

			else {
				int temp_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

				if(connect(temp_socket, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
					tm_win_print(tab->window_text, "Couldn't connect to server %s\n", input_args[1]);
				}

				else {
#ifdef _WIN32
					closesocket(tab->server_socket);
#else
					close(tab->server_socket);
#endif
					tab->server_socket = temp_socket;

					send(tab->server_socket, input_args[2], 256, 0);
					tm_win_print(tab->window_text, "Successfully connected to server %s\n", input_args[1]);
					tab->connected = 1;
				}

#ifdef _WIN32
				FreeAddrInfo(servinfo);
#else
				freeaddrinfo(servinfo);
#endif
			}
		}
	}

	else if(strcmp(input_args[0], "/list") == 0) {
		if(tab->connected) {
			send(tab->server_socket, "LIST", 4, 0);
		}
	}
	
	else if(strcmp(input_args[0], "/clear") == 0) {
			tm_win_clear(tab->window_text);
	}

	free(input_args);
	tm_win_attrib(tab->window_text, TM_ATTRIB_FG_BRIGHTCYAN, 0);
}

