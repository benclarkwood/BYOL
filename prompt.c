#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpc.h"

#ifdef _WIN32

static char buffer[2048];

/* Fake readline function */
char* readline(char* prompt) {
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char* cpy = malloc(strlen(buffer)+1);
	strcpy(cpy, buffer);
	cpy[strlen(cpy)-1] = '\0';
	return cpy;
}

/* Fake add_history function */
void add_history(char* unused) {};

#elif __linux__

/* Include the editline/history.h header for Linux systems */
#include <editline/history.h>

#else

/* Include the editline/readline header  for both Mac and Linux systems*/
#include <editline/readline.h>

#endif

int main(int argc, char** argv) {
	/* Print version and exit information */
	puts("Benny Version 0.0.0.0.1");
	puts("Type exit to end the session.\n");
	
	/* In a never ending loop */
	while (1) {
		/* Output our prompt and get input */
		char* input = readline("benny> ");
		
		/* Check for exit */
		if (strcmp(input, "exit") == 0) {
			break;
		}
		
		/* Add input to history */
		add_history(input);
		
		/* Echo input back to user */
		printf("No you're a %s\n", input);
		
		/* Free retrieved input */
		free(input);
	}
	
	return 0;
}