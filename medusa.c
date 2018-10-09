#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

#include <editline/readline.h>
#include <editline/history.h>


//Enumeração para os tipos de erros
//enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };
enum { LERR_BAD_OP, LERR_BAD_NUM };
//Enumeração para as tipos de dado em lval
enum { LVAL_NUM, LVAL_ERR };

// lval = lisp evaluation
//Estrutura base para o eval
typedef struct {
  int type;
  long num;
  int err;
} lval;

//Numero
lval lval_num(long x) {
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

//Erro
lval lval_err(int x) {
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}

//imprime a estrutura lval para debug e lidar com erros
void lval_print(lval v) {
  switch (v.type) {
    //Se for um numero, imprima
    case LVAL_NUM: printf("%li", v.num); break;

    //Se for um erro
    case LVAL_ERR:
      //Qual erro?
      /*
      if (v.err == LERR_DIV_ZERO) {
        printf("Error: Division By Zero!");
      }
      */
      if (v.err == LERR_BAD_OP)   {
        printf("Error: Invalid Operator!");
      }
      if (v.err == LERR_BAD_NUM)  {
        printf("Error: Invalid Number!");
      }
    break;
  }
}

//Print line do lisp
void lval_println(lval v) { lval_print(v); putchar('\n'); }

/* Operações*/
lval eval_op(lval x, char* op, lval y) {

  //Se for erro, retorna o erro
  if (x.type == LVAL_ERR) return x;
  if (y.type == LVAL_ERR) return y;

  //Operações
  if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
  if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
  if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
  if (strcmp(op, "/") == 0) { return (y.num == 0) ?  lval_num(y.num) :  lval_num(x.num / y.num); }
  if (strcmp(op, "max") == 0) { return (x.num > y.num) ?  lval_num(x.num) :  lval_num(y.num); }
  if (strcmp(op, "min") == 0) { return (x.num < y.num) ?  lval_num(x.num) :  lval_num(y.num); }


  return lval_err(LERR_BAD_OP);

}
lval eval(mpc_ast_t* t) {

  /* Se for numero converte com atoi(melhorado) e retorna */
  if (strstr(t->tag, "number")) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  /* Grava o terceiro filho em `x` */
  char* op = t->children[1]->contents;
  lval x = eval(t->children[2]);

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
      number   : /-?[0-9]+/;                             \
      operator : '+' | '-' | '*' | '/' | \"max\" | \"min\" ;         \
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
      lval result = eval(r.output);
      lval_println(result);
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
