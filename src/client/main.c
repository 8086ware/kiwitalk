#include <sockmanip.h>
#include <termmanip.h>
#include <string.h>
#include "tab.h"

int main(void) {
	sm_win_init();
	tm_init();

	struct Tab** tabs = NULL;
	int amount = 0;

	new_tab(&tabs, &amount);

	int tab_selected = 0;

	char command[4096];

	while(1) {
		get_tab_input(&tabs, tab_selected, command, 4096);
		process_tab_input(&tabs, &tab_selected, &amount, command, 4096);

		receive_tab_server_requests(&tabs, &tab_selected, &amount);

		tm_win_update(tabs[tab_selected]->window_text);
	}

	tm_exit();
}
