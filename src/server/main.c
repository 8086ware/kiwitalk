#include <sockmanip.h>
#include <termmanip.h>
#include <string.h>
#include <stdio.h>
#include "broadcast.h"

#define KIWITALK_PORT "47831"

int main(void) {
	tm_init();
	tm_flags(TM_FLAG_SCROLL, 1);

	sm_win_init();
	Sm_server* server = sm_server(NULL, KIWITALK_PORT, SM_SERVER_CREATE, SM_TCP, SM_IPV4);

	if(server == NULL) {
		tm_print("Couldn't host server on port %s. Is another program using the port?", KIWITALK_PORT);
		tm_input_ch();
		return 1;
	}

	tm_print("Listening on port %s\n", KIWITALK_PORT);
	tm_update();

	char** names = NULL;

	while(1) {
		if(sm_new_client(server, 0)) {
			names = realloc(names, sizeof(char*) * sm_get_server_client_amount(server));

			if(names == NULL) {
				fprintf(stderr, "Memory error");
				return 2;
			}

			names[sm_get_server_client_amount(server) - 1] = malloc(256);

			if(names[sm_get_server_client_amount(server) - 1] == NULL) {
				fprintf(stderr, "Memory error");
				return 3;
			}

			sm_receive(sm_get_client_socket(*sm_get_server_client(server, sm_get_server_client_amount(server) - 1)), names[sm_get_server_client_amount(server) - 1], 256, -1);

			char ip[256];

			getnameinfo(&sm_get_server_client(server, sm_get_server_client_amount(server) - 1)->addr, sm_get_server_client(server, sm_get_server_client_amount(server) - 1)->addr_len, ip, 128, NULL, 0, 0);

			char send_buf[4096];
			int bytes_to_send = sprintf(send_buf, "JOIN_%s_%s", names[sm_get_server_client_amount(server) - 1], ip);
			broadcast_to_clients(server, &names, send_buf, bytes_to_send);
			tm_update();
		}

		char send_buf[4096];
		int bytes_to_send = 0;

		for(int i = 0; i < sm_get_server_client_amount(server); i++) {
			char receive_buf[4096];
			int bytes_received = sm_receive(sm_get_client_socket(*sm_get_server_client(server, i)), receive_buf, 4096, 0);

			receive_buf[bytes_received] = '\0';

			if(bytes_received > 0) {
				bytes_to_send = sprintf(send_buf, "<%s> %s", names[i], receive_buf);
				broadcast_to_clients(server, send_buf, bytes_to_send);
			}
		}
	}
}
