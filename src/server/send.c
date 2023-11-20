#include "send.h"
#include <termmanip.h>
#include <stdio.h>

void send_to_client(Sm_server* s, int client_id, char*** names, char* string, int bytes) {
	tm_print("Sending to: %s | %s\n", (*names)[client_id], string);
	tm_update();

	if(sm_send(sm_get_client_socket(*sm_get_server_client(s, client_id)), string, bytes, 0) == -1) {
	}
}
