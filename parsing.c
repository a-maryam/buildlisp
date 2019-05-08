/* 
 * File: parsing.c
 */
#include "mpc.h"

#ifdef _WIN32

static char buffer[2048];
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
/* ERROR HANDLING */
// new lisp value (lval) struct decl.
// Lispy evaluation yields either a number or an error
typedef struct {
  int type; // type and err are booleans
  long num; // naming these makes the program more coherent
  int err;
} lval;

// enumeration of possible lval types
enum {LVAL_NUM, LVAL_ERR };
// enumeration of possible error types
enum {LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM};

lval lval_num(long x) {
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

lval lval_err(int x) {
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}
// Printing lisp values
void lval_print(lval v) {
  switch(v.type) {
  case LVAL_NUM: printf("%li", v.num); break;
  case LVAL_ERR:
    if(v.err == LERR_DIV_ZERO) {
      printf("Error: Division by zero.");
    }
    if(v.err == LERR_BAD_OP) {
      printf("Error: Invalid Operator.");
    }
    if(v.err == LERR_BAD_NUM) {
      printf("Error: Invalid Number!");
    }
    break;
  }
}
// print lisp value and then newline:
void lval_println(lval v) {lval_print(v); putchar('\n');}

lval eval_op(lval x, char* op, lval y) {
  // return errors
  if(x.type == LVAL_ERR) return x;
  if(y.type == LVAL_ERR) return y;

  // otherwise compute
  if(strcmp(op, "+") == 0) return lval_num(x.num + y.num);
  if(strcmp(op, "-") == 0) return lval_num(x.num - y.num);
  if(strcmp(op, "*") == 0) return lval_num(x.num * y.num);
  if(strcmp(op, "/") == 0) {
    return y.num == 0 ?
      lval_err(LERR_DIV_ZERO) :
      lval_num(x.num / y.num);
  }
  return lval_err(LERR_BAD_OP);
}
// Recursive function
lval eval(mpc_ast_t* t) {
  
  // tag contains number
  if(strstr(t->tag, "number")) {
    // check for error
    errno = 0; // <errno.h> in included in mpc.h
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  char * op = t -> children[1]-> contents; //whoa repeated arrow operators!
  lval x = eval(t -> children[2]);

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
      lval result = eval(r.output);
      //printf("%li\n", result);
      lval_println(result);
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
