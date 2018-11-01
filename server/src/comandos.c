#include "comandos.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool verify_file_existence(char *file) {
  FILE *f = fopen(file, "r");

  if (f == NULL) {
    return 1;
  } else {
    return 0;
  }
}

void save_settings(char *filename) {
  FILE *f = fopen(filename, "w");
  if (f == NULL) {
    printf("Erro com o ficheiro %s\n", filename);
    fclose(f);
    return;
  } else {
    for (int i = 0; i < editor.lines; i++) {
      for (int j = 0; j < editor.columns; j++)
        fprintf(f, "%c", editor.content[i][j]);
    }
    fprintf(f, "\n");
  }
  fclose(f);
}

void settings() {
  printf("Numero de Linhas: %d\nNumero de colunas: %d\nNumero Max. de "
         "Utilizadores: %d\nNome do Pipe Principal: %s\n",
         editor.lines, editor.columns, max_users, PIPE);
}
void load_settings(char *filename) { printf("OLA\n"); }
void cmd(char *com) {
  char **arg = NULL;
  char *p = strtok(com, " ");
  int n_spaces = 0;

  /* dividir a string e anexar tokens em 'arg' */
  while (p) {
    arg = realloc(arg, sizeof(char *) * ++n_spaces);
    if (arg == NULL)
      exit(-1); /* falha na alocação de meḿória */
    arg[n_spaces - 1] = p;
    p = strtok(NULL, " ");
  }

  /* Realocar um elemento extra para o último NULL */
  arg = realloc(arg, sizeof(char *) * (n_spaces + 1));
  arg[n_spaces] = 0;

  if (strcmp(arg[0], "load") == 0) {
    if (arg[1]) {
      if (verify_file_existence(arg[1]) == 0)
        load_settings(arg[1]);
      else {
        printf("Ficheiro nao encontrado!\n");
        return;
      }
    } else {
      printf("Faltam argumentos!\n");
      return;
    }
  } else if (strcmp(arg[0], "save") == 0) {
    if (arg[1])
      save_settings(arg[1]); // guardar definições em ficheiro .txt
    else {
      printf("Faltam argumentos!\n");
      return;
    }
  } /*else if (strcmp(arg[0], "free") == 0) {
    if (arg[1])
      free_row(atoi(arg[1])); // libertar/apagar o conteúdo de determinada linha
    else {
      printf("Faltam argumentos!\n");
      return;
    }
  } else if (strcmp(arg[0], "statistics") == 0)
    statistics();
  else if (strcmp(arg[0], "users") == 0)
    users();
  else if (strcmp(arg[0], "text") == 0)
    text();
  else if (strcmp(arg[0], "shutdown") == 0)
    shutdown();
  else if (strcmp(arg[0], "settings") == 0)
    settings(); */
  else {
    printf("Comando inválido!\n");
    return;
  }
}