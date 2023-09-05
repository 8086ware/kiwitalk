#include <string.h>
#include <stdlib.h>

char** parse_command(char* string, int* amount, char* delim) {
	char** request_args = NULL;
	int request_arg_amount = 0;

	char* temp = NULL;

	do {
		if(request_arg_amount == 0) {
			temp = strtok(string, delim);
		}

		else {
			temp = strtok(NULL, delim);
		}

		if(temp != NULL) {
			request_args = realloc(request_args, sizeof(char*) * (request_arg_amount + 1));
			request_args[request_arg_amount] = temp;
			request_arg_amount++;
		}
	} while(temp != NULL);

	(*amount) = request_arg_amount;
	return request_args;
}
