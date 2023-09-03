#include "broadcast.h"
#include <termmanip.h>
#include <stdio.h>

void broadcast_to_clients(Sm_server* s, char* string, int bytes) {
	string[bytes] = '\0';
	tm_print("(SENDING TO ALL CLIENTS) %s\n", string);
	tm_update();

	for(int i = 0; i < sm_get_server_client_amount(s); i++) {
		sm_send(sm_get_client_socket(*sm_get_server_client(s, i)), string, bytes, 0);
	}
}
