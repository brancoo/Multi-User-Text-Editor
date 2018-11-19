#include "comandos.h"
#include "main.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

bool verify_file_existence(char *file) {
  FILE *f = fopen(file, "r");

  if (f == NULL) {
    return 1;
  } else {
    return 0;
  }
}

void shutdown() {
  char pipe[20];
  int fd;
  temp.action = SERVER_SHUTDOWN;
  sprintf(pipe, "../pipe-%d", temp.pid);
  fd = open(pipe, O_WRONLY, 0600);

  write(fd, &temp, sizeof(temp));
  close(fd);
  unlink(PIPE);
  printf("Programa terminado\n");
  exit(0);
}

void save_settings(char *filename) {
  FILE *f = fopen(filename, "w");
  int i, j;
  if (f == NULL) {
    printf("Erro com o ficheiro %s\n", filename);
    fclose(f);
    return;
  } else {
    for (i = 0; i < editor.lines; i++) {
      for (j = 0; j < editor.columns; j++)
        fprintf(f, "%c", editor.content[i][j]);
    }
  }
  fclose(f);
}

void users() {
  char pipe[20];
  sprintf(pipe, "pipe-%d", temp.pid);
  printf("Utilizador: %s\n", temp.user);
  printf("Nome do Pipe: %s\n", pipe);
}

void settings() {
  printf("Numero de Linhas: %d\nNumero de colunas: %d\nNumero Max. de "
         "Utilizadores: %d\nNome do Pipe Principal: %s\n",
         editor.lines, editor.columns, max_users, PIPE);
}

void load_file(char *filename) {
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    printf("Erro ao carregar ficheiro : %s\n", filename);
    return;
  }

  for (int x = 0; x < editor.lines; x++) {
    for (int y = 0; y < editor.columns; y++) {
      fscanf(file, "%c", &editor.content[x][y]);
    }
  }
  fclose(file);
}

void text() {
  for (int i = 0; i < editor.lines; i++) {
    for (int j = 0; j < editor.columns; j++) {
      printf("%c", editor.content[i][j]);
    }
    printf("\n");
  }
}

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
        load_file(arg[1]);
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
  } else if (strcmp(arg[0], "shutdown") == 0)
    shutdown();
  else if (strcmp(arg[0], "settings") == 0)
    settings();
  else if (strcmp(arg[0], "sair") == 0)
    shutdown();
  else if (strcmp(arg[0], "users") == 0)
    users();
  else if (strcmp(arg[0], "text") == 0)
    text();
  /*else if (strcmp(arg[0], "free") == 0) {
   if (arg[1])
     free_row(atoi(arg[1])); // libertar/apagar o conteúdo de determinada linha
  else { printf("Faltam argumentos!\n"); return;
   }
 } else if (strcmp(arg[0], "statistics") == 0)
   statistics();*/
  else {
    printf("Comando inválido!\n");
    return;
  }
  free(arg);
}