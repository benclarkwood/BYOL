#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
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

/* Structs */

// lval
typedef struct {
	int type;
	long num;
	int err;
} lval;

/* Enumerations */
enum { LVAL_NUM, LVAL_ERR };

enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Function declarations */
lval eval_op(lval, char*, lval);
lval eval(mpc_ast_t*);

lval lval_num(long);
lval lval_err(int);

void lval_print(lval);
void lval_println(lval);

int main(int argc, char** argv) {
	/* Create some parsers */
	mpc_parser_t* Number = mpc_new("number");
	mpc_parser_t* Operator = mpc_new("operator");
	mpc_parser_t* Expr = mpc_new("expr");
	mpc_parser_t* Lispy = mpc_new("lispy");
	
	/* Define them with the following language */
	mpca_lang(MPC_LANG_DEFAULT, 
		"                                                       \
			number   : /-?[0-9]+/ ;                             \
			operator : '+' | '-' | '*' | '/' | '%' | '^' ;      \
			expr     : <number> | '(' <operator> <expr>+ ')' ;  \
			lispy    : /^/ <operator> <expr>+ /$/ ;             \
		", 
		Number, Operator, Expr, Lispy);
	
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
		
		/* Attempt to parse the user input */
		mpc_result_t r;
		
		if (mpc_parse("<stdin>", input, Lispy, &r)) {
			/* On success print the AST */
			lval result = eval(r.output);
			lval_println(result);
			mpc_ast_delete(r.output);
		} else {
			/* Otherwise print the error */
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}
		
		/* Free retrieved input */
		free(input);
	}
	
	/* Undefine and delete our parsers */
	mpc_cleanup(4, Number, Operator, Expr, Lispy);
	
	return 0;
}

lval eval(mpc_ast_t* t) {
	/* If tagged as a number return it directly, otherwise it is an expression */
	if (strstr(t->tag, "number")) {		
		long x = strtol(t->contents, NULL, 10);
		
		printf("%i", errno);
		
		return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
	}
	
	/* The operator will be the second child */
	char* op = t->children[1]->contents;
		
	/* Store the third child in 'x' */
	lval x = eval(t->children[2]);
		
	/* Iterate the remaining children, combining using the operator */
	int i = 3;
	
	while (strstr(t->children[i]->tag, "expr")) {
		x = eval_op(x, op, eval(t->children[i]));
		i++;
	}
	
	/* If op is '-' and i still 3, negate x */
	if (i == 3 && strcmp(op, "-") == 0) {
		return lval_num(-x.num);
	}
	
	return x;
}

lval eval_op(lval x, char* op, lval y) {
	/* If either value is an error return it */
	if (x.type == LVAL_ERR) { return x; }
	if (y.type == LVAL_ERR) { return y; }
	
	if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
	if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
	if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
	if (strcmp(op, "/") == 0) { 
		return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
	}
	if (strcmp(op, "%") == 0) { return lval_num(x.num % y.num); }
	if (strcmp(op, "^") == 0) { return lval_num(pow(x.num,y.num)); }
	
	return lval_err(LERR_BAD_OP);
}

/* Create a new number type lval */
lval lval_num(long x) {
	lval v;
	v.type = LVAL_NUM;
	v.num = x;
	return v;
}

/* Create a new error type lval */
lval lval_err(int x) {
	lval v;
	v.type = LVAL_ERR;
	v.err = x;
	return v;
}

/* Print an lval */
void lval_print(lval v) {
	switch (v.type) {
		/* In the case the type is a number, print it, then 'break' out of the switch' */
		case LVAL_NUM: printf("%li", v.num); break;
		
		/* In the case the type is an error */
		case LVAL_ERR:
			/* Check what type of error and print correct one */
			if (v.err == LERR_DIV_ZERO) { printf("Error: Division by Zero!"); }
			if (v.err == LERR_BAD_OP) { printf("Error: Invalid Operator!"); }
			if (v.err == LERR_BAD_NUM) { printf("Error: Invalid Number!"); }
		break;
	}
}

/* Print an lval followed by a newline */
void lval_println(lval v) { lval_print(v); putchar('\n'); }