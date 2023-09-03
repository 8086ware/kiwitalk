#ifndef BROADCAST_H
#define BROADCAST_H
#include <sockmanip.h>

void broadcast_to_clients(Sm_server* s, char* string, int bytes);

#endif
