#include "broadcast.h"
#include <termmanip.h>
#include <stdio.h>

void broadcast_to_clients(Sm_server* s, char*** names, char* string, int bytes) {
	string[bytes] = '\0';
	tm_print("(SENDING TO ALL CLIENTS) %s\n", string);
	tm_update();

	for(int i = 0; i < sm_get_server_client_amount(s) ;i++) {
		if(sm_send(sm_get_client_socket(*sm_get_server_client(s, i)), string, bytes, 0) == -1) {
			char send_left_buf[4096];
			int bytes_send_left = sprintf(send_left_buf, "LEFT_%s", (*names)[i]);

			char* temp = (*names)[sm_get_server_client_amount(s) - 1];
			(*names)[i] = temp;

			*names = realloc(*names, (sm_get_server_client_amount(s) - 1) * sizeof(char*));
			sm_client_free(s, i);
			broadcast_to_clients(s, names, send_left_buf, bytes_send_left);
		}
	}
}
