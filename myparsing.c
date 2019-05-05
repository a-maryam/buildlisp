// compile using: cc -std=c99 -Wall prompt.c -ledit -o prompt
//#include "mpc.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <editline/history.h>
//static char input[2048];
//if compiling in Windows, compile these functions
#ifdef _WIN32
#include <string.h>
static char input[2048];
// made up readline function:
char * readline(char * prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char * cpy = malloc(strlen(buffer) + 1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}
void add_history(char * unused) {}
#else
#include <editline/readline.h>
#endif
#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char** argv) {

  puts("Lispy Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");

  while(1) {
    //fputs("lispy> ", stdout);
    //fgets(input, 2048, stdin);
    char * input = readline("lispy> ");
    add_history(input);
    printf("No you're a %s\n", input);
    free(input);
  }
  return 0;
}
