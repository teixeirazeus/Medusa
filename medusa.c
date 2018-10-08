#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

int main(int argc, char** argv) {

  /* Imprime informacao sobre versao e como sair */
  puts("Medusa Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");

  /* Num laco que nunca acaba */
  while (1) {

    /* Imprima nosso prompt e obtem entrada */
    char* input = readline(">>> ");

    /* Adiciona a entrada ao historico */
    add_history(input);

    /* Ecoa a entrada de volta ao usuario */
    printf("No you're a %s\n", input);

    /* Libera a entrada obtida */
    free(input);

  }

  return 0;
}
