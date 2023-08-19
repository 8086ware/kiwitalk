#include <sockmanip.h>
#include <termmanip.h>
#include <string.h>
#include <stdio.h>

#define KIWITALK_PORT "47831"

int main(void) {
	tm_init();
	tm_flags(TM_FLAG_SCROLL, 1);

	Sm_server* server = sm_server(NULL, KIWITALK_PORT, SM_SERVER_CREATE, SM_TCP);

	if(server == NULL) {
		tm_print("Couldn't host server on port %s. Is another program using the port?", KIWITALK_PORT);
		tm_input_ch();
		return 1;
	}

	tm_print("Listening on port %s\n", KIWITALK_PORT);
	tm_update();

	while(1) {
		if(sm_new_client(server, 10)) {
			struct sockaddr_storage addr;
			memset(&addr, 0, sizeof(addr));
			socklen_t addr_len = sizeof(addr);

			tm_attrib(TM_ATTRIB_FG_GREEN, 1);

			if(getsockname(sm_get_client_socket(*sm_get_server_client(server, sm_get_server_client_amount(server) - 1)), (struct sockaddr*)&addr, &addr_len) != -1) {
				char ip[128];
				char port[16];

				if(getnameinfo((struct sockaddr*)&addr, addr_len, ip, 128, port, 16, NI_NUMERICHOST) == 0) {
					tm_print("Connection from %s:%s\n", ip, port);
				}
			}
	
			else {
				tm_print("Connection from Unknown Host (Couldn't get socket name!\n");
			}

			tm_attrib(TM_ATTRIB_FG_GREEN, 0);
			tm_update();
		}
	}
}
