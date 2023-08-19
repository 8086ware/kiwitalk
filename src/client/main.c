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

	Tm_window* main_window_text = tm_window(1, 1, tm_win_get_columns(main_window) - 2, tm_win_get_rows(main_window) - 2);
	Tm_window* main_window_input = tm_window(0, tm_win_get_rows(main_window) - 1, tm_win_get_columns(main_window), 1);

	tm_win_parent(main_window, main_window_text, TM_CHILD_NORMAL);
	tm_win_parent(main_window, main_window_input, TM_CHILD_NORMAL);

	tm_win_print(main_window_text, "-- KIWITALK --\n\nWelcome. Type /help for list of commands.");
	
	tm_win_update(main_window_text);

	while(menu) {
		char command[1024];

		tm_win_clear(main_window_input);
		tm_win_print(main_window_input, "> ");
		tm_win_input_str(main_window_input, command, 1024);
		tm_win_print(main_window_text, "\n");

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
			tm_win_print(main_window_text, "COMMANDS:\n\nconnect [HOSTNAME] - Will ask for a server hostname to connect to\nexit - Exit kiwitalk");
		}

		else if(strcmp(arg1, "/exit") == 0) {
			tm_win_free(main_window);
			tm_exit();
			return 0;
		}

		else if(strcmp(arg1, "/connect") == 0) {
			tm_win_print(main_window_text, "Attempting to connect...\n");
			tm_win_update(main_window);

			server = sm_server(arg2, KIWITALK_PORT, SM_SERVER_CONNECT, SM_TCP);

			if(server == NULL) {
				tm_win_print(main_window_text, "Couldn't connect to server %s.\n", arg2);
			}

			else {
				connected = 1;
			}
		}

		if(connected) {
			
		}

		tm_win_update(main_window);
	}

	tm_exit();
}
