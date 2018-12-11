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
Editor clients[3];
int active_users = 0;

void getMAX_USERS(int n) {
  srand(time(NULL));
  editor.max_users = rand() % n;
}

bool check_if_users_exceeds_max_active() {
  if (active_users >= editor.max_users) {
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

updateAllUsersEditor() {
  for (int i = 0; i < active_users; i++) {
    for (int j = 0; j < editor.lines; j++) {
      for (int k = 0; k < editor.columns; k++) {
        clients[i].content[j][k] = editor.content[j][k];
        clients[i].action = UPDATE;
        clients[i].num_chars = editor.num_chars;
      }
    }
  }
}

void update_all_users() {
  int fd;
  char pipe[20];
  updateAllUsersEditor();
  for (int i = 0; i < active_users; i++) {

    sprintf(pipe, "../pipe-%d", clients[i].pid);
    fd = open(pipe, O_WRONLY, 0600);
    // editor.action = UPDATE;
    write(fd, &clients[i], sizeof(clients[i]));

    close(fd);
  }
}

void add_to_active_users_list(int pid, char username[8]) {
  clients[active_users].pid = pid;
  strcpy(clients[active_users].username, username);
  clients[active_users].editing_line = -1;
  clients[active_users].status = false;
  active_users++;
}

bool verify_line_edition(Editor aux) {
  for (int i = 0; i < active_users; i++) {

    if (clients[i].editing_line == aux.editing_line &&
        clients[i].status == true) {
      return false;
    }
  }
  return true;
}

bool check_users_existence(char username[8]) {
  for (int i = 0; i < active_users; i++) {
    if (strcmp(username, clients[i].username) == 0) {
      return false;
    }
  }
  return true;
}

void delete_user_from_array(int pid) {
  for (int i = 0; i < active_users; i++) {
    if (clients[i].pid == pid) {
      for (int j = i; j < active_users; j++) {
        clients[j] = clients[j + 1];
      }
    }
  }
  active_users--;
}

void *receiver() {
  Editor receive, send;
  int fd, fd_send;
  char pipe[20];

  mkfifo(PIPE, 0600);
  fd = open(PIPE, O_RDWR);

  do {
    read(fd, &receive, sizeof(receive));
    strcpy(editor.username, receive.username);
    editor.pid = receive.pid;
    sprintf(pipe, "../pipe-%d", receive.pid);
    fd_send = open(pipe, O_WRONLY, 0600);
    switch (receive.action) {
    case LOGIN:
      printf("numero de users: %d\n", active_users);
      if (check_users_existence(receive.username) ==
          true) { // CRIAR OUTRO DEFINE
        if (find_username(receive.username, "../out/medit.db") == true) {

          if (check_if_users_exceeds_max_active() == true) {
            add_to_active_users_list(receive.pid, receive.username);
            send.action = LOGGED; // LOGIN EFECTUADO COM SUCESSO
            send.editing_line = -1;
            send.status = false;
            printf("User %s com o PID %d iniciou sessao!\n", receive.username,
                   receive.pid);
            printf("numero de users logados: %d\n", active_users);

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
      } else {
        printf("ja logado\n");
        send.action = USER_ALREADY_LOGGED; // USERNAME JA LOGADO
        write(fd_send, &send, sizeof(send));
      }
      break;
    case CLIENT_SHUTDOWN:
      delete_user_from_array(receive.pid);
      printf("O utilizador com o PID %d saiu do programa!\n", receive.pid);
      break;
    case UPDATE:
      for (int i = 0; i < editor.lines; i++) {
        for (int j = 0; j < editor.columns; j++) {
          editor.content[i][j] = receive.content[i][j];
        }
      }
      editor.num_chars = receive.num_chars;

      if (receive.status == false) {

        for (int i = 0; i < active_users; i++) {
          if (clients[i].pid == receive.pid) {
            clients[i].editing_line = receive.editing_line;
            clients[i].status = receive.status;

          }
        }
      }

      update_all_users();

      break;
    case ASK_PERMISSION:
      if (verify_line_edition(receive) == true) {
        for (int i = 0; i < active_users; i++) {
          if (clients[i].pid == receive.pid) {
            clients[i].editing_line = receive.editing_line;
            clients[i].status = true;
          }
        }
        receive.action = PERMISSION_ACCEPTED;
        receive.status = true;
        printf(" DEU ask permission \n");
        write(fd_send, &receive, sizeof(receive));
      } else {
        receive.editing_line = -1;
        receive.action = PERMISSION_DENIED;
        receive.status = false;
        write(fd_send, &receive, sizeof(receive));
      }
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
  char file[80], comando[80];
  int opt, fd_pipe, res;
  pthread_t thread;
  // fd_pipe, filehandler para o pipe principal
  // opt, serve para ajudar a ler os argumentos opcionais da linha de comandos
  // res = auxiliar para criar thread
  signal(SIGINT, SIGhandler);
  signal(SIGHUP, SIGhandler);

  if ((opt = getopt(argc, argv, "f:")) != -1) {
    if (optarg) {
      if (verify_file_existence(argv[2]) == true)
        strcpy(file, argv[2]);
      else {
        printf("Base de dados de usernames: ");
        scanf("%s", file);
        if (verify_file_existence(argv[2]) == false)
          strcpy(file, "../out/medit.db");
      }
    } else {
      printf("Base de dados de usernames: ");
      scanf("%s", file);
      if (verify_file_existence(argv[2]) == false)
        strcpy(file, "../out/medit.db"); // valor por defeito!
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
  load_file("../out/text.txt");
  system("clear"); // LIMPA A CONSOLA
  printf("Servidor iniciado!\n");

  // Thread para auxiliar leitura de dados do cliente
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