#include "editor.h"
#include "estruturas.h"
#include <ctype.h>
#include <curses.h>
#include <fcntl.h>
#include <getopt.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define WIDTH 47
#define HEIGHT 17

WINDOW *my_win;
WINDOW *info;
Editor receive;
int logged = 0; // para saber se o user se conseguiu logar com sucesso
int stop = 0;

// caso seja o cliente a fechar em 1º lugar (sem estar loggado)
void client_shutdown() {
  char pipe[20];
  sprintf(pipe, "../pipe-%d", getpid());
  unlink(pipe);
  printf("\nPrograma terminado!\n");
  exit(0);
}

// caso seja o servidor a fechar em 1º lugar
void server_shutdown() {
  char pipe[20];
  sprintf(pipe, "../pipe-%d", getpid());
  unlink(pipe);
  printw("Programa terminado!\nPressione qualquer tecla para sair");
  refresh();
  sleep(2);
  endwin();
  exit(0);
}

// caso o cliente esteja loggado, ele encerra e envia mensagem ao servidor
void shutdown() {
  char pipe[20];
  int fd;
  Editor send;
  if (logged == 1) {
    send.pid = getpid();
    send.action = CLIENT_SHUTDOWN;
    sprintf(pipe, "../pipe-%d", getpid());
    fd = open(PIPE, O_WRONLY, 0600);
    write(fd, &send, sizeof(send));
    unlink(pipe);
    clear();
    printw("Cliente saiu da sessão!\nPressione qualquer tecla para sair");
    getch();
    refresh();
    endwin();
    exit(0);
  } else {
    client_shutdown();
  }
}

void *receiver() {
  char pipe[20];
  int fd_pipe;

  sprintf(pipe, "../pipe-%d", getpid());
  mkfifo(pipe, 0600);

  fd_pipe = open(pipe, O_RDWR);
  do {
    read(fd_pipe, &receive, sizeof(receive));
    switch (receive.action) {
    case SERVER_SHUTDOWN: // SERVIDOR TERMINOU
      clear();
      printw("O servidor encerrou!\n");
      refresh();
      server_shutdown();
      break;
    case LOGGED: // LOGIN DO CLIENTE COM SUCESSO
      logged = 1;
      read(fd_pipe, &receive, sizeof(receive));
      break;
    case NOT_LOGGED: // USERNAME NAO ENCONTRADO NA BASE DE DADOS
      printf("Username invalido\n");
      break;
    case MAX_ACTIVE_USERS:
      printf("Numero maximo de utilizadores activos atingido!\n");
      break;
    case FREE:
      print_content(my_win, receive.content);
      break;
    }
  } while (1);
  close(fd_pipe);
  unlink(pipe);
  pthread_exit(0);
}

void SIGhandler(int sig) {
  signal(sig, SIG_IGN);
  shutdown();
}

void alarme(int sig) { stop = 1; }

int main(int argc, char **argv) {
  Editor temp;
  char pipe[20], npipe[20];
  int opt, fd, res;
  pthread_t task;

  signal(SIGINT, SIGhandler);
  signal(SIGHUP, SIGhandler);
  signal(SIGALRM, alarme);
  system("clear");

  while ((opt = getopt(argc, argv, "u:p:")) != -1) {
    switch (opt) {
    case 'u':
      if (optarg) // vai analisar se -u foi introduzido pelo user e guarda-o
        strcpy(temp.username, optarg); // FALTA VERIFICAR LOGIN AQUI !!!!
      break;
    case 'p':
      if (optarg) // vai analisar se -p foi introduzido pelo user
        strcpy(pipe, optarg);
      else { // senão existir argumento opcional, toma o valor por omissão
        strcpy(pipe, PIPE);
      }
      break;
    }
  }

  temp.pid = getpid();
  sprintf(npipe, "../pipe-%d", temp.pid);

  if (access(PIPE, F_OK) != 0) {
    printf("O servidor nao se encontra em execucao. A sair...\n");
    getch();
    exit(0);
  }

  if (mkfifo(npipe, S_IRWXU) == -1) {
    printf("Erro ao criar pipe. A sair...\n");
    getch();
    exit(0);
  }

  res = pthread_create(&task, NULL, &receiver, NULL);
  if (res != 0) {
    perror("ERRO!A criar a thread!!!\n");
    unlink(pipe);
    getch();
    exit(1);
  }

  fd = open(PIPE, O_RDWR); // abrir para leitura/escrita
  if (fd == -1) {
    printf("Erro a abrir o pipe do servidor. A sair...\n");
    getch();
    exit(0);
  }

  do {
    printf("Username:"); // senão existir então é pedido explicitamente
    scanf("%9s", temp.username);
    temp.action = LOGIN; // flag LOGIN para o servidor saber o que fazer
    write(fd, &temp, sizeof(temp));
    sleep(1);
  } while (logged == 0);

  int ch;
  int x = 1;
  int y = 1;

  initscr();
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_RED);

  cbreak();
  keypad(stdscr, TRUE); // para ativar a leitura das setas
  noecho();

  printw("Bem Vindo:%s\tPress Esc to exit\t", temp.username);
  refresh();

  my_win = create_win(HEIGHT, WIDTH, y, x);
  info = create_win(5, WIDTH, HEIGHT + 1, 1);

  wmove(my_win, y, x);

  print_content(my_win, receive.content);

  mvwprintw(info, 1, 1, "Editor chars:");
  mvwprintw(info, 1, 14, "%d", receive.num_chars);

  receive.n_chars = 0;
  mvwprintw(info, 3, 1, "User chars:");
  mvwprintw(info, 3, 12, "%d", receive.n_chars);

  mvwprintw(info, 1, 30, "Modo Navegação");
  wrefresh(info);
  for (y = 1; y <= MAX_LINES; y++) {
    x = 49;
    mvprintw(y + 1, x, "Linha %2d", y);
  }
  x = 1;
  y = 1;
  wmove(my_win, y, x); // Start with cursor in 1 1
  refresh();
  wrefresh(my_win);
  
  while ((ch = getch()) != 27) // sai ciclo quando clicar escape
  {
    char s[MAX_COLUMNS];
    for (int i = 0; i < MAX_COLUMNS; i++) {
      s[i] = receive.content[y - 1][i];
    }

    int lengh = receive.num_chars;

    switch (ch) {
    case KEY_LEFT:
      if (x > 1) {
        x--;
      }
      break;
    case KEY_RIGHT:
      if (x < 45) // colunas
      {
        x++;
      }
      break;
    case KEY_UP:
      if (y > 1) // linhas
      {
        y--;
      }
      break;
    case KEY_DOWN:
      if (y < 15) {
        y++;
      }
      break;
    case 10:
      mvwprintw(info, 1, 30, "                ");
      mvwprintw(info, 1, 30, "Modo Edição");
      wrefresh(info);
      attron(COLOR_PAIR(1));
      mvprintw(y + 1, 58, "%s", temp.username);
      refresh();
      wmove(my_win, y, x); // mexer o cursor para a posição actual
      wrefresh(my_win);
      receive.status = true;
      receive.editing_line = y;
      write(fd, &receive, sizeof(receive));
      alarm(3);
      while ((ch = getch()) != 10) {
        alarm(0);

        if (ch == 27 || stop == 1) {
          stop = 0;
          recovery_array(my_win, s, receive.content, y, x);
          mvprintw(y + 1, 58, "        ");
          refresh();
          receive.status = false;
          receive.num_chars = lengh;
          write(fd, &receive, sizeof(receive));
          break;
        }

        switch (ch) {
        case KEY_DC:        // delete
        case KEY_BACKSPACE: // backspace
        case 8:             // delete
        case 127:           // backspace
          delete_char(my_win, receive.content, x, y);
          break;
        case KEY_LEFT:
          if (x > 1) {
            x--;
          }
          break;
        case KEY_RIGHT:
          if (x < 45) // colunas
          {
            x++;
          }
          break;
        case KEY_UP:
          break;
        case KEY_DOWN:
          break;
        default:
          add_char(my_win, receive.content, ch, x, y);
          if (x < 45) {
            x++;
          }
        }
        wmove(my_win, y, x);
        mvwprintw(info, 1, 14, "%d", receive.num_chars);
        mvwprintw(info, 3, 12, "%d", receive.n_chars);
        wrefresh(info);
        wrefresh(my_win);
        alarm(3);
      }
    }
    attroff(COLOR_PAIR(1));
    mvwprintw(info, 1, 30, "Modo Navegação");
    wrefresh(info);
    mvprintw(y + 1, 58, "        ");
    refresh();
    wmove(my_win, y, x);
    mvwprintw(info, 1, 14, "%d", receive.num_chars);
    wrefresh(info);
    wrefresh(my_win);

    receive.action = UPDATE; // envia o conteúdo actualizado para o servidor
    write(fd, &receive, sizeof(receive));
  }
  endwin();

  pthread_join(task, NULL);
  close(fd);
  unlink(pipe);
  return 0;
}
