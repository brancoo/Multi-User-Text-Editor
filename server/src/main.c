#include "main.h"
#include "comandos.h"
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

Editor editor;
int active_users;

void getMAX_USERS(int n) {
  srand(time(NULL));
  editor.max_users = rand() % n;
}

bool check_if_users_exceeds_max_active() {
  if (active_users > editor.max_users) {
    return false; // ATINGIDO O LIMITE MÁXIMO DE USERS
  } else {
    return true; // PODE FAZER LOGIN
  }
}

bool find_username(char *username, char *filename) {
  FILE *f = fopen(filename, "r");
  char buffer[9];

  if (f == NULL) {
    printf("ERRO AO ABRIR FICHEIRO!\n");
    fclose(f);
    return false;
  }

  while (fscanf(f, "%8s", buffer) == 1) {
    if (strcmp(buffer, username) == 0) {
      fclose(f);
      return true;
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
    editor.max_users = MEDIT_MAXUSERS;
  else {
    getMAX_USERS(editor.lines);
  }
  editor.num_chars = 0;
  editor.cursor.x = 4;
  editor.cursor.y = 5;
}

void *receiver() {
  Editor receive, send;
  int fd, fd_send;
  char pipe[20];

  mkfifo(PIPE, 0600);
  fd = open(PIPE, O_RDWR);

  do {
    read(fd, &receive, sizeof(receive));
    sprintf(pipe, "../pipe-%d", receive.pid);
    fd_send = open(pipe, O_WRONLY, 0600);
    switch (receive.action) {
    case LOGIN:
      if (find_username(receive.username, "../out/medit.db") == true) {
        if (check_if_users_exceeds_max_active() == true) {
          send.action = LOGGED; // LOGIN EFECTUADO COM SUCESSO
          strcpy(editor.username, receive.username);
          editor.pid = receive.pid;
          printf("User %s iniciou sessao!\n", receive.username);
          load_file("../out/text.txt");
          write(fd_send, &send, sizeof(send));
          write(fd_send, &editor, sizeof(editor));
        } else {
          send.action = MAX_ACTIVE_USERS;
          write(fd_send, &send, sizeof(send));
        }
      } else {
        send.action = NOT_LOGGED; // USERNAME NAO ENCONTRADO NA BASE DE DADOS
        write(fd_send, &send, sizeof(send));
      }
      break;
    case CLIENT_SHUTDOWN:
      printf("O utilizador %d -> %s saiu do programa!\n", receive.pid,
             receive.username);
      break;
    }
  } while (1);
  close(fd);
  pthread_exit(0);
}

void SIGhandler(int sig) {
  signal(sig, SIG_IGN);
  shutdown();
}

int main(int argc, char *argv[]) {
  char *file, comando[80], pipe[20];
  int opt, fd_pipe, n_named_pipes, res;
  pthread_t thread;
  // fd_pipe, filehandler para o pipe principal
  // opt, serve para ajudar a ler os argumentos opcionais da linha de comandos
  // n_named_pipes, numero de named pipes de interação
  // res = auxiliar para criar thread
  signal(SIGINT, SIGhandler);

  // saber se o admin enviou pela linha de comandos
  while ((opt = getopt(argc, argv, "f:p:n:")) != -1) {
    switch (opt) {
    case 'f':
      if (optarg) { // se existir argumento e se o ficheiro existir
        if (verify_file_existence(argv[2]) == 0)
          file = argv[2];
      } else {
        printf("Base de dados de usernames: ");
        scanf("%s", file);
        if (verify_file_existence(argv[2]) != 0)
          file = "../out/medit.db"; // valor por defeito!
      }
      break;
    case 'p':
      if (optarg)             // vai analisar se -p foi introduzido pelo user
        strcpy(pipe, optarg); // copia o valor do argumento para a variável pipe
      else { // senão existir argumento opcional, toma o valor por omissão
        strcpy(pipe, PIPE);
      }
      break;
    case 'n':
      if (optarg)
        n_named_pipes = atoi(argv[2]);
      else { // assumimos por defeito, o valor 3 (=MAXUSERS)
        n_named_pipes = MEDIT_MAXUSERS;
      }
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

  // vai abrir o pipe para leitura/escrita
  fd_pipe = open(PIPE, O_RDWR);
  if (fd_pipe == -1) {
    printf("Erro ao abrir pipe. A sair...\n");
    exit(0);
  }

  system("clear"); // LIMPA A CONSOLA
  printf("Servidor iniciado!\n");

  // Thread serve para ler duas entradas de dados (comandos e cliente)
  res = pthread_create(&thread, NULL, &receiver, NULL);
  if (res != 0) {
    perror("ERRO! A criar a thread!!!\n");
    exit(1);
  }

  while (1) {
    scanf(" %79[^\n]s", comando);
    if (comando[strlen(comando) - 1] == '\n')
      comando[strlen(comando) - 1] = '\0';
    if (strcmp(comando, " ") != 0)
      cmd(comando);
  }

  pthread_join(thread, NULL);
  close(fd_pipe);
  unlink(PIPE);
  free(file);

  return 0;
}
