#include "main.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

Editor editor;
int max_users;

bool verify_file_existence(char *file) {
  FILE *f = fopen(file, "r");

  if (f == NULL) {
    return 1;
  } else {
    return 0;
  }
}

int getMAX_USERS(int n) { // vai ser preciso para o MAX_USERS
  srand(time(NULL));
  return rand() % n;
}

bool find_username(char *username, char *filename) {
  FILE *f = fopen(filename, "rt");
  char c, buffer[50];

  if (f == NULL) {
    printf("ERRO AO ABRIR FICHEIRO!\n");
    return false;
  }

  while ((c = getc(f)) != EOF) {
    fscanf(f, "%s", buffer);
    if (strcmp(buffer, username) == 0) {
      return true;
      fclose(f);
    }
  }
  fclose(f);
  return false;
}

void verify_env_var() {
  // se a variável de ambiente não existe, então assumimos o valor por omissão
  // (=15 linhas)
  if (getenv("MEDIT_MAXLINES") == NULL)
    editor.lines = MAX_LINES;
  else {
    editor.lines = atoi(getenv("MEDIT_MAXLINES"));
  }
  // se a variável de ambiente não existe, então assumimos o valor por omissão
  // (=45 colunas)
  if (getenv("MEDIT_MAXCOLUMNS") == NULL)
    editor.columns = MAX_COLUMNS;
  else {
    editor.columns = atoi(getenv("MEDIT_MAXCOLUMNS"));
  }

  if (getenv("MEDIT_TIMEOUT") == NULL)
    editor.timeout = MEDIT_TIMEOUT;
  else {
    editor.timeout = atoi(getenv("MEDIT_TIMEOUT"));
  }
  if (getenv("MEDIT_MAXUSERS") == NULL)
    max_users = MEDIT_MAXUSERS;
  else {
    max_users = getMAX_USERS(editor.lines);
  }
}

int main(int argc, char *argv[]) {
  char *file, pipe[10];
  int opt;

  // saber se o admin enviou pela linha de comandos
  while ((opt = getopt(argc, argv, "f:p:")) != -1) {
    switch (opt) {
    case 'f':
      if (optarg && (verify_file_existence(argv[2]) == 0)) //se existir argumento e se existir o ficheiro
            file = argv[2]; 
     else {  //caso não exista argumento ou nao se encontre o ficheiro 
        file = "medit.db";
        printf("Base de dados assumida por defeito: %s\n", file);
      }
      break;
    case 'p':
      if (optarg) // vai analisar se -p foi introduzido pelo user
        strcpy(pipe, optarg); // copia o valor do argumento para a variável pipe
      else {
        strcpy(pipe,PIPE); // senão existir argumento opcional, toma o valor por omissão
      }
      break;
    }
  }
  
  verify_env_var();
  return 0;
}
