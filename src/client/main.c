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

		else if(c == '\b' || c == '\127') {
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
