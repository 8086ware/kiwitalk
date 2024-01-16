#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include <string.h>

struct Server_config open_config() {
	struct Server_config sc = {0};

	FILE* config_file = fopen("server_config.kt", "r");

	if (config_file != NULL) {
		char buffer[8192];

		fread(buffer, sizeof(char), 8192, config_file);

		char* title = malloc(4096);

		strcpy(title, strtok(buffer, "|||"));

		sc.server_title = title;

		fclose(config_file);
	}
	return sc;
}
