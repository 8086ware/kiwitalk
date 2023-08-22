#include <sockmanip.h>
#include <termmanip.h>
#include <string.h>

#define KIWITALK_PORT "47831"

int main(void) {
	sm_win_init();
	tm_init();
	
	int scr_cols, scr_rows;

	tm_get_scrsize(&scr_cols, &scr_rows);
	_Bool menu = 1;
	_Bool connected = 0;

	Sm_server* server = NULL;

	Tm_window* main_window = tm_window(0, 0, scr_cols, scr_rows);
	tm_win_flags(main_window, TM_FLAG_SCROLL, 1);
	tm_win_flags(main_window, TM_FLAG_INPUTBLOCK | TM_FLAG_ECHO, 0);

	tm_win_border(main_window);

	Tm_window* main_window_text = tm_window(1, 1, tm_win_get_columns(main_window) - 2, tm_win_get_rows(main_window) - 3);
	Tm_window* main_window_input = tm_window(1, tm_win_get_rows(main_window) - 2, tm_win_get_columns(main_window) - 2, 1);

	tm_win_parent(main_window, main_window_text, TM_CHILD_NORMAL);
	tm_win_parent(main_window, main_window_input, TM_CHILD_NORMAL);

	tm_win_print(main_window_text, "-- KIWITALK --\n\nWelcome. Type /help for list of commands.\n");

	tm_win_update(main_window_text);

	int input_amount = 0;
	_Bool finished_input = 0;

	tm_win_print(main_window_input, "> ");
	while(menu) {
		char command[1024];

		int c = tm_win_input_ch(main_window_input);

		if(c == '\n' || c == '\r') {
			command[input_amount] = '\0';
			finished_input = 1;
			input_amount = 0;
		}

		else if(c == '\b' || c == '\177') {
			if(input_amount > 0) {
				input_amount--;

				command[input_amount] = '\0';

				int temp_x = tm_win_get_cursor_x(main_window_input);
				int temp_y = tm_win_get_cursor_y(main_window_input);
				tm_win_cursor(main_window_input, temp_x - 1, temp_y);
				tm_win_print(main_window_input, " ");
				tm_win_cursor(main_window_input, temp_x - 1, temp_y);
			}
		}

		else if(c != 0) {
			tm_win_print(main_window_input,"%c", c);
			tm_win_update(main_window_input);
			command[input_amount] = c;
			input_amount++;
		}

		if(finished_input) {
			finished_input = 0;

			tm_win_clear(main_window_input);
			tm_win_print(main_window_input, "> ");

			if(*command == '/') {
				char* arg1;
				char* arg2;
				char* arg3;
				char* arg4;
				char* arg5;

				arg1 = strtok(command, " ");
				arg2 = strtok(NULL, " ");
				arg3 = strtok(NULL, " ");
				arg4 = strtok(NULL, " ");
				arg5 = strtok(NULL, " ");

				if(strcmp(arg1, "/help") == 0) {
					tm_win_print(main_window_text, "COMMANDS:\n\nconnect [HOSTNAME] [USERNAME] - Will ask for a server hostname to connect to\nexit - Exit kiwitalk\n");
				}

				else if(strcmp(arg1, "/exit") == 0) {
					tm_win_free(main_window);
					tm_exit();
					return 0;
				}

				else if(strcmp(arg1, "/connect") == 0) {
					if(arg2 == NULL || arg3 == NULL) {
						tm_win_print(main_window_text, "No username or IP entered\n");
						tm_win_update(main_window);
					}

					else {
						tm_win_print(main_window_text, "Attempting to connect...\n");
						tm_win_update(main_window);

						server = sm_server(arg2, KIWITALK_PORT, SM_SERVER_CONNECT, SM_TCP, SM_IPV4);

						if(server == NULL) {
							tm_win_print(main_window_text, "Couldn't connect to server %s.\n", arg2);
						}

						else {
							sm_send(sm_get_server_socket(server), arg3, 256, -1);
							connected = 1;
						}
					}
				}
			}

			else if(connected) {
				if(*command != '\0') {
					sm_send(sm_get_server_socket(server), command, strlen(command) + 1, 0);
				}
			}
		}

		if(connected) {
			char receive_buffer[4096];
			int receive_bytes = sm_receive(sm_get_server_socket(server), receive_buffer, 4096, 0);

			receive_buffer[receive_bytes] = '\0';

			if(receive_bytes > 0) {
				tm_win_attrib(main_window_text, TM_ATTRIB_FG_BRIGHTGREEN, 1);
				tm_win_print(main_window_text, receive_buffer);
				tm_win_print(main_window_text, "\n");
				tm_win_attrib(main_window_text, TM_ATTRIB_FG_BRIGHTGREEN, 0);
			}
		}

		tm_win_update(main_window);
	}

	tm_exit();
}
