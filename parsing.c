/* Took source file because of compilation issues
 * I think the problem was actually errors in my makefile which I believe
 * are fixed
 * File: evaluation.c
 */
#include "mpc.h"

#ifdef _WIN32

static char buffer[2048];

// new lval struct decl.
typedef struct {
  int type; // type and err are booleans
  long num; // naming these makes the program more coherent
  int err;
} lval;

// enumeration of possible lval types
enum {LVAL_NUM, LVAL_ERR };
// enumeration of possible error types
enum {LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM};


char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0'; // why? strcpy gives you a null term. doesn't it?
  return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#include <histedit.h>
#endif
// returns zero if op is not in our set of ops
long eval_op(long x, char* op, long y) {
  if(strcmp(op, "+") == 0) return x+y;
  if(strcmp(op, "-") == 0) return x-y;
  if(strcmp(op, "*") == 0) return x*y;
  if(strcmp(op, "/") == 0) return x/y;
  return 0;
}
// Recursive function
long eval(mpc_ast_t* t) {
  
  // tag contains number
  if(strstr(t->tag, "number")) {
    return atoi(t -> contents);
  }

  char * op = t -> children[1]-> contents; //whoa repeated arrow operators!
  long x = eval(t -> children[2]);

  // iterate through the rest of the children...
  int i = 3;
  
  while(strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t-> children[i]));
    i++;
  } // I need to think through this recursion
  
  return x;
}
  

int main(int argc, char** argv) {
  
  /* Create Some Parsers */
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Lispy    = mpc_new("lispy");
  
  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number   : /-?[0-9]+/ ;                             \
      operator : '+' | '-' | '*' | '/' ;                  \
      expr     : <number> | '(' <operator> <expr>+ ')' ;  \
      lispy    : /^/ <operator> <expr>+ /$/ ;             \
    ",
    Number, Operator, Expr, Lispy);
  
  puts("Lispy Version 0.0.0.0.2");
  puts("Press Ctrl+c to Exit\n");
  
  while (1) {
  
    char* input = readline("lispy> ");
    add_history(input);
    
    /* Attempt to parse the user input */
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      /* On success print and delete the AST */
      //  mpc_ast_print(r.output);
      //mpc_ast_delete(r.output);
      long result = eval(r.output);
      printf("%li\n", result);
      mpc_ast_delete(r.output);
      
    } else {
      /* Otherwise print and delete the Error */
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    
    free(input);
  }
  
  /* Undefine and delete our parsers */
  mpc_cleanup(4, Number, Operator, Expr, Lispy);
  
  return 0;
}
