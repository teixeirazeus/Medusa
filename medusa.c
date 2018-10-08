#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

#include <editline/readline.h>
#include <editline/history.h>

/* Operações*/
long eval_op(long x, char* op, long y) {
  if (strcmp(op, "+") == 0) { return x + y; }
  if (strcmp(op, "-") == 0) { return x - y; }
  if (strcmp(op, "*") == 0) { return x * y; }
  if (strcmp(op, "/") == 0) { return x / y; }
  return 0;
}

long eval(mpc_ast_t* t) {

  /* Se for numero converte com atoi e retorna */
  if (strstr(t->tag, "number")) {
    return atoi(t->contents);
  }

  /* O operador é sempre o segundo filho  */
  char* op = t->children[1]->contents;

  /* Grava o terceiro filho em `x` */
  long x = eval(t->children[2]);

  /* Itera os filhos que sobraram e junta */
  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;
}



int main(int argc, char** argv) {

  /* Inicia os nós */
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Atom = mpc_new("atom");

  /* Definição de expressões regulares */
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number   : /-?[0-9]+/ ;                             \
      operator : '+' | '-' | '*' | '/' ;                  \
      expr     : <number> | '(' <operator> <expr>+ ')' ;  \
      atom    : /^/ <operator> <expr>+ /$/ ;             \
    ",
    Number, Operator, Expr, Atom);

  /* Imprime informacao sobre versao e como sair */
  puts("Medusa Version 0.0.0.0.3");
  puts("Press Ctrl+c to Exit\n");

  /* Num laco que nunca acaba */
  while (1) {

    /* Imprima nosso prompt e obtem entrada */
    char* input = readline(">>> ");

    /* Adiciona a entrada ao historico */
    add_history(input);

    /* Parser */
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Atom, &r)) {

      /*Avalia, imprime, deleta*/
      long result = eval(r.output);
      printf("%li\n", result);
      mpc_ast_delete(r.output);

    } else {
      /* Erros */
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    /* Libera a entrada obtida */
    free(input);

  }
  
  mpc_cleanup(4, Number, Operator, Expr, Atom);

  return 0;
}
