#include <string.h>
#include "tab.h"

#ifndef _WIN32
#include <signal.h>
void sigpipe_hndlr() {}
#endif

int main(void) {
#ifdef _WIN32
	WSADATA d;
	WSAStartup(MAKEWORD(2, 2), &d);
#else
	signal(SIGPIPE, sigpipe_hndlr);
#endif
	tm_set_title("kiwitalk");

	Tm_terminal* terminal = tm_terminal();

	struct Tab** tabs = NULL;
	int amount = 0;

	new_tab(terminal, &tabs, &amount);

	int tab_selected = 0;

	char command[4096];

	while(1) {
		tm_win_update(tabs[tab_selected]->window);
		get_tab_input(&tabs, tab_selected, amount, command, 4096);
		process_tab_input(&tabs, &tab_selected, &amount, command, 4096);

		for(int i = 0; i < amount; i++) {
			receive_tab_server_requests(&tabs, &i, &amount);
		}
	}
}
