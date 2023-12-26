#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <termmanip.h>
#include <string.h>
#include <stdio.h>
#include "parse_command.h"
#include <time.h>
#include <stdlib.h>

#define KIWITALK_PORT "47831"

#ifndef _WIN32
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
void sigpipe_hndlr() {}
#endif

#include "config.h"

int main(void) {
	struct Server_config sc = open_config();
#ifdef _WIN32
	WSADATA d;

	WSAStartup(MAKEWORD(2, 2), &d);
#else
	signal(SIGPIPE, sigpipe_hndlr);
#endif

	tm_init();
	tm_flags(TM_FLAG_SCROLL, 1);

	struct addrinfo hints, *servinfo = NULL;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if(getaddrinfo(NULL, KIWITALK_PORT, &hints, &servinfo) == -1) {
		fprintf(stderr, "GAI Error");
		return 1;
	}

	int server_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	if(bind(server_socket, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		fprintf(stderr, "bind() error");
		return 1;
	}

	if(listen(server_socket, 100) == -1) {
		fprintf(stderr, "listen() error");
		return 1;
	}

	tm_print("Listening on port %s\nTitle: %s\n", KIWITALK_PORT, sc.server_title);
	tm_update();

	char** names = NULL;

	int poll_amount = 1;

	struct pollfd* s_poll = malloc(sizeof(struct pollfd));

	if(s_poll == NULL) {
		fprintf(stderr, "Memory error");
		return 1;
	}

	s_poll[0].fd = server_socket;
	s_poll[0].events = POLLIN;

	while(1) {
		tm_update();
		for(int i = 0; i < poll_amount; i++) {
			s_poll[i].revents = 0;
		}
#ifdef _WIN32
		WSAPoll(s_poll, poll_amount, -1);
#else
		poll(s_poll, poll_amount, -1);
#endif

		if(s_poll[0].revents & POLLIN) {
			names = realloc(names, sizeof(char*) * (poll_amount + 1));

			if(names == NULL) {
				fprintf(stderr, "Memory error");
				return 2;
			}

			poll_amount++;

			names[poll_amount - 1] = malloc(256);

			if(names[poll_amount - 1] == NULL) {
				fprintf(stderr, "Memory error");
				return 3;
			}

			struct sockaddr client_addr;
			int client_len = sizeof(client_addr);

			int client_socket = accept(server_socket, &client_addr, &client_len);

			s_poll = realloc(s_poll, sizeof(struct pollfd) * (poll_amount + 1));

			if(s_poll == NULL) {
				fprintf(stderr, "Memory error");
				return 4;
			}

			s_poll[poll_amount - 1].fd = client_socket;
			s_poll[poll_amount - 1].events = POLLIN;

			recv(client_socket, names[poll_amount - 1], 256, 0);

			char ip[256];

			getnameinfo(&client_addr, client_len, ip, 128, NULL, 0, 0);

			char send_buf[4096];
			int bytes_to_send = sprintf(send_buf, "JOIN\x1d%s\x1d", names[poll_amount - 1]);

			tm_print("%s Joined server from %s\n", names[poll_amount - 1], ip);

			for(int i = 1; i < poll_amount; i++) {
				send(s_poll[i].fd, send_buf, bytes_to_send, 0);
			}

			tm_update();
		}

		char send_buf[4096];
		int bytes_to_send = 0;

		for(int i = 1; i < poll_amount; i++) {
			char receive_buf[4096];
			if(s_poll[i].revents & POLLIN || s_poll[i].revents == 3) {
				int bytes_received = recv(s_poll[i].fd, receive_buf, 4096, 0);

				receive_buf[bytes_received] = '\0';

				if(bytes_received > 0) {
					int request_arg_amount = 0;
					char** request_args = parse_command(receive_buf, &request_arg_amount, "\177");

					if(strcmp(request_args[0], "MSG") == 0) {
						bytes_to_send = sprintf(send_buf, "MSG\x1d%s\x1d%s\x1d", names[i], request_args[1]);
						tm_print("Sending out MSG %s %s\n", names[i], request_args[1]);

						for(int i = 1; i < poll_amount; i++) {
							send(s_poll[i].fd, send_buf, bytes_to_send, 0);
						}
					}

					else if(strcmp(request_args[0], "LIST") == 0) {
						bytes_to_send += sprintf(send_buf, "LIST\x1d");

						tm_print("Sending out LIST %s\n", names[i]);

						for(int i = 1; i < poll_amount; i++) {
							bytes_to_send += sprintf(send_buf + bytes_to_send, "%s\x1d", names[i]);
						}

						send(s_poll[i].fd, send_buf, bytes_to_send, 0);
					}

					else if(strcmp(request_args[0], "TITLE") == 0) {
						bytes_to_send += sprintf(send_buf, "TITLE\x1d%s\x1d", sc.server_title);

						tm_print("Sending out TITLE %s\n", names[i]);

						send(s_poll[i].fd, send_buf, bytes_to_send, 0);
					}

					free(request_args);
				}

				if(bytes_received <= 0) {
#ifdef _WIN32
					closesocket(s_poll[i].fd);
#else
					close(s_poll[i].fd);	
#endif
					bytes_to_send = sprintf(send_buf, "LEFT\177%s", names[i]);

					tm_print("Sending out LEFT %s\n", names[i]);

					char* temp = names[poll_amount - 1];
					names[i] = temp;
					names = realloc(names, (poll_amount - 1) * sizeof(char*));

					if(names == NULL) {
						fprintf(stderr, "Memory error");
						return 5;
					}

					struct pollfd temp2 = s_poll[poll_amount - 1];
					s_poll[i] = temp2;
					s_poll = realloc(s_poll, (poll_amount - 1) * sizeof(struct pollfd));

					if(s_poll == NULL) {
						fprintf(stderr, "Memory error");
						return 6;
					}

					poll_amount--;

					for(int k = 1; k < poll_amount; k++) {
						send(s_poll[k].fd, send_buf, bytes_to_send, 0);
					}
				}
			}
		}
	}
}

