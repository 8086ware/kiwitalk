#include "tab.h"
#include <termmanip.h>
#include <sockmanip.h>
#include <string.h>

void receive_tab_server_requests(struct Tab*** tabs, int* tab_selected, int* amount) {
	struct Tab* tab = (*tabs)[*tab_selected];

	if(tab->connected) {
		char receive_buffer[4096];
		int receive_bytes = sm_receive(sm_get_server_socket(tab->server), receive_buffer, 4096, 0);

		receive_buffer[receive_bytes] = '\0';

		if(receive_bytes > 0) {
			char** request_args = NULL;
			int request_arg_amount = 0;

			char* temp = NULL;

			do {
				if(request_arg_amount == 0) {
					temp = strtok(receive_buffer, "_");
				}

				else {
					if(strcmp(request_args[0], "MSG") == 0 && request_arg_amount == 2) {
						temp = strtok(NULL, "\177");
					}

					else {
						temp = strtok(NULL, "_");
					}
				}

				if(temp != NULL) {
					request_args = realloc(request_args, sizeof(char*) * (request_arg_amount + 1));
					request_args[request_arg_amount] = temp;
					request_arg_amount++;
				}
			} while(temp != NULL);


			if(strcmp(request_args[0], "MSG") == 0) {
				tm_win_attrib(tab->window_text, TM_ATTRIB_FG_CYAN, 1);
				tm_win_print(tab->window_text, "%s ~> %s\n", request_args[1], request_args[2]);
				tm_win_attrib(tab->window_text, TM_ATTRIB_FG_CYAN, 0);
			}

			if(strcmp(request_args[0], "JOIN") == 0) {
				tm_win_attrib(tab->window_text, TM_ATTRIB_FG_GREEN, 1);
				tm_win_print(tab->window_text, "-> %s joined from %s\n", request_args[1], request_args[2]);
				tm_win_attrib(tab->window_text, TM_ATTRIB_FG_GREEN, 0);
			}
		}
	}
}
