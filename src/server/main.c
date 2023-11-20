#include <sockmanip.h>
#include <termmanip.h>
#include <string.h>
#include <stdio.h>
#include "parse_command.h"
#include <signal.h>
#include "send.h"

#define KIWITALK_PORT "47831"

void sigpipe_hndlr() {}

int main(void) {
#ifndef _WIN32
	signal(SIGPIPE, sigpipe_hndlr);
#endif

	tm_init();
	tm_flags(TM_FLAG_SCROLL, 1);

	sm_win_init();
	Sm_server* server = sm_server(NULL, KIWITALK_PORT, SM_SERVER_CREATE, SM_TCP, SM_IPV4);

	if(server == NULL) {
		tm_print("Couldn't host server on port %s. Is another program using the port?", KIWITALK_PORT);
		tm_input();
		return 1;
	}

	tm_print("Listening on port %s\n", KIWITALK_PORT);
	tm_update();

	char** names = NULL;

	while(1) {
		if(sm_new_client(server, 1)) {
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
			int bytes_to_send = sprintf(send_buf, "JOIN\177%s\177%s\177", names[sm_get_server_client_amount(server) - 1], ip);

			for(int i = 0; i < sm_get_server_client_amount(server); i++) {
				send_to_client(server, i, &names, send_buf, bytes_to_send);
			}

			tm_update();
		}

		char send_buf[4096];
		int bytes_to_send = 0;

		for(int i = 0; i < sm_get_server_client_amount(server); i++) {
			char receive_buf[4096];
			int bytes_received = sm_receive(sm_get_client_socket(*sm_get_server_client(server, i)), receive_buf, 4096, 0);

			receive_buf[bytes_received] = '\0';

			if(bytes_received > 0) {
				int request_arg_amount = 0;
				char** request_args = parse_command(receive_buf, &request_arg_amount, "\177");

				if(strcmp(request_args[0], "MSG") == 0) {
					bytes_to_send = sprintf(send_buf, "MSG\177%s\177%s\177", names[i], request_args[1]);


					for(int i = 0; i < sm_get_server_client_amount(server); i++) {
						send_to_client(server, i, &names, send_buf, bytes_to_send);
					}
				}

				else if(strcmp(request_args[0], "LIST") == 0) {
					bytes_to_send += sprintf(send_buf, "LIST\177");

					for(int i = 0; i < sm_get_server_client_amount(server); i++) {
						bytes_to_send += sprintf(send_buf + bytes_to_send, "%s\177", names[i]);
					}

					send_to_client(server, i, &names, send_buf, bytes_to_send);
				}

				free(request_args);
			}

			else {
				if(sm_send(sm_get_client_socket(*sm_get_server_client(server, i)), 0, 0, 0) == -1) {
					char send_left_buf[4096];
					int bytes_send_left = sprintf(send_left_buf, "LEFT\177%s", names[i]);

					char* temp = names[sm_get_server_client_amount(server) - 1];
					names[i] = temp;

					names = realloc(names, (sm_get_server_client_amount(server) - 1) * sizeof(char*));
					sm_client_free(server, i);

					for(int i = 0; i < sm_get_server_client_amount(server); i++) {
						send_to_client(server, i, &names, send_left_buf, bytes_send_left);
					}
				}
			}
		}
	}
}
