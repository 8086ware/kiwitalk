#include <sockmanip.h>
#include <termmanip.h>
#include <string.h>
#include <stdio.h>

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
				tm_print("Memory error");
				tm_input_ch();
				tm_exit();
				return 2;
			}

			names[sm_get_server_client_amount(server) - 1] = malloc(256);

			if(names[sm_get_server_client_amount(server) - 1] == NULL) {
				tm_print("Memory error");
				tm_input_ch();
				tm_exit();
				return 3;
			}

			sm_receive(sm_get_client_socket(*sm_get_server_client(server, sm_get_server_client_amount(server) - 1)), names[sm_get_server_client_amount(server) - 1], 256, -1);

			struct sockaddr_storage addr;
			memset(&addr, 0, sizeof(addr));
			socklen_t addr_len = sizeof(addr);

			tm_attrib(TM_ATTRIB_FG_GREEN, 1);

			if(getsockname(sm_get_client_socket(*sm_get_server_client(server, sm_get_server_client_amount(server) - 1)), (struct sockaddr*)&addr, &addr_len) != -1) {
				char ip[128];
				char port[16];

				if(getnameinfo((struct sockaddr*)&addr, addr_len, ip, 128, port, 16, 0) == 0) {
					tm_print("Connection from %s:%s with username %s\n", ip, port, names[sm_get_server_client_amount(server) - 1]);
				}
			}
	
			else {
				tm_print("Connection from Unknown Host (Couldn't get socket name!\n");
			}

			tm_attrib(TM_ATTRIB_FG_GREEN, 0);
			tm_update();
		}

		for(int i = 0; i < sm_get_server_client_amount(server); i++) {
			char receive_buf[4096];
			int bytes_received = sm_receive(sm_get_client_socket(*sm_get_server_client(server, i)), receive_buf, 4096, 0);

			if(bytes_received > 0) {
				char send_buf[4096];
				int bytes_to_send = sprintf(send_buf, "<%s> %s", names[i], receive_buf);

				tm_print("(SENDING TO ALL CLIENTS) %s\n", send_buf);
				tm_update();

				for(int i = 0; i < sm_get_server_client_amount(server); i++) {
					sm_send(sm_get_client_socket(*sm_get_server_client(server, i)), send_buf, bytes_to_send, 0);
				}
			}
		}
	}
}
