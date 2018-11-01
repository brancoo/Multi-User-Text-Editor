#include "main.h"
#include "comandos.h"
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

Editor editor;
int max_users;

int getMAX_USERS(int n) { // vai ser preciso para o MAX_USERS
  srand(time(NULL));
  return rand() % n;
}

bool find_username(char *username, char *filename) {
  FILE *f = fopen(filename, "rt");
  char c, buffer[50];

  if (f == NULL) {
    printf("ERRO AO ABRIR FICHEIRO!\n");
    fclose(f);
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
  editor.screenrows = 0;
  editor.num_chars = 0;
  editor.cursor.x = 4;
  editor.cursor.y = 5;
}

int main(int argc, char *argv[]) {
  char *file, comando[80], pipe[10];
  int opt, fd_pipe;
  // fd_pipe, filehandler para o pipe
  // opt, serve para ajudar a ler os argumentos opcionais da linha de comandos

  // saber se o admin enviou pela linha de comandos
  while ((opt = getopt(argc, argv, "f:p:")) != -1) {
    switch (opt) {
    case 'f':
      if (optarg) {
        if (verify_file_existence(argv[2]) ==
            0) // se existir argumento e se o ficheiro existir
          file = argv[2];
      } else { // caso não exista o argumento do nome do ficheiro
        printf("Base de dados de usernames: ");
        scanf("%s", file);
        if (verify_file_existence(argv[2]) !=
            0) // caso o nome dado pelo admin nao exista, atribuímos a base de
               // dados default
          file = "../out/medit.db";
      }
      break;
    case 'p':
      if (optarg)             // vai analisar se -p foi introduzido pelo user
        strcpy(pipe, optarg); // copia o valor do argumento para a variável pipe
      else {
        strcpy(
            pipe,
            PIPE); // senão existir argumento opcional, toma o valor por omissão
      }
      break;
    }
  }
  verify_env_var();

  if (access(PIPE, F_OK) == 0) { // verifica se já existe o PIPE
    printf("Já se encontra um servidor em execução. A sair...\n");
    exit(0);
  }
  if (mkfifo(PIPE, S_IRWXU) < 0) { // cria o pipe
    printf("Erro ao criar pipe. A sair...\n");
    exit(0);
  }

  fd_pipe = open(PIPE, O_RDWR); // vai abrir o pipe para leitura/escrita
  if (fd_pipe == -1) {
    printf("Erro ao abrir ficheiro. A sair...\n");
    exit(0);
  }
  printf("Servidor iniciado!\n");

  while (1) {
    scanf(" %79[^\n]s", comando);
    if (comando[strlen(comando) - 1] == '\n')
      comando[strlen(comando) - 1] = '\0';
    if (strcmp(comando, " ") != 0)
      cmd(comando);
    printf("Comando: %s\n", comando);
  }
  close(fd_pipe);
  unlink(PIPE);
  return 0;
}
