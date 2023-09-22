#include "send.h"
#include <termmanip.h>
#include <stdio.h>

void send_to_client(Sm_server* s, int client_id, char*** names, char* string, int bytes) {
	tm_print("Sending to: %s | %s\n", (*names)[client_id], string);
	tm_update();

	if(sm_send(sm_get_client_socket(*sm_get_server_client(s, client_id)), string, bytes, 0) == -1) {
		char send_left_buf[4096];
		int bytes_send_left = sprintf(send_left_buf, "LEFT\177%s", (*names)[client_id]);

		char* temp = (*names)[sm_get_server_client_amount(s) - 1];
		(*names)[client_id] = temp;

		*names = realloc(*names, (sm_get_server_client_amount(s) - 1) * sizeof(char*));
		sm_client_free(s, client_id);

		for(int i = 0; i < sm_get_server_client_amount(s); i++) {
			send_to_client(s, i, names, send_left_buf, bytes_send_left);
		}
	}
}
