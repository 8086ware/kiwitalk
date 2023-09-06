#ifndef BROADCAST_H
#define BROADCAST_H
#include <sockmanip.h>

void send_to_client(Sm_server* s, int client_id, char*** names, char* string, int bytes);

#endif
