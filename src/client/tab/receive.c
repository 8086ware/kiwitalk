#include "tab.h"
#include <termmanip.h>
#include <string.h>
#include "parse_command.h"
#include <time.h>

void receive_tab_server_requests(struct Tab*** tabs, int* tab_selected, int* amount) {
	struct Tab* tab = (*tabs)[*tab_selected];

	if(tab->connected) {
		char receive_buffer[4096];
		int receive_bytes = recv(tab->server_socket, receive_buffer, 4096, 0);

		receive_buffer[receive_bytes] = '\0';

		if(receive_bytes > 0) {
			int request_arg_amount = 0;
			char** request_args = parse_command(receive_buffer, &request_arg_amount, "\x1d");

			time_t e_time;
			time(&e_time);
			struct tm* time = localtime(&e_time);

			if(strcmp(request_args[0], "MSG") == 0) {
				tm_win_attrib(tab->window_text, TM_ATTRIB_FG_BRIGHTWHITE, 1);
				tm_win_print(tab->window_text, "[%d:%d:%d] ", time->tm_hour, time->tm_min, time->tm_sec);
				tm_win_print(tab->window_text, "%s ~> %s\n", request_args[1], request_args[2]);
				tm_win_attrib(tab->window_text, TM_ATTRIB_FG_BRIGHTWHITE, 0);
			}

			if(strcmp(request_args[0], "JOIN") == 0) {
				tm_win_attrib(tab->window_text, TM_ATTRIB_FG_GREEN, 1);
				tm_win_print(tab->window_text, "[%d:%d:%d] ", time->tm_hour, time->tm_min, time->tm_sec);
				tm_win_print(tab->window_text, "-> %s joined\n", request_args[1], request_args[2]);
				tm_win_attrib(tab->window_text, TM_ATTRIB_FG_GREEN, 0);
			}

			if(strcmp(request_args[0], "LIST") == 0) {
				tm_win_attrib(tab->window_text, TM_ATTRIB_FG_YELLOW, 1);
				for(int i = 1; i < request_arg_amount; i++) {
					tm_win_print(tab->window_text, "%s\n", request_args[i]);
				}
				tm_win_attrib(tab->window_text, TM_ATTRIB_FG_YELLOW, 0);
			}

			if(strcmp(request_args[0], "LEFT") == 0) {
				tm_win_attrib(tab->window_text, TM_ATTRIB_FG_RED, 1);
				tm_win_print(tab->window_text, "[%d:%d:%d] ", time->tm_hour, time->tm_min, time->tm_sec);
				tm_win_print(tab->window_text, "<- %s left\n", request_args[1]);
				tm_win_attrib(tab->window_text, TM_ATTRIB_FG_RED, 0);
			}

			free(request_args);
		}
	}
}
