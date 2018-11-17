#include "estruturas.h"
//#include <curses.h>
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

Editor editor;
aux receive;
int logged = 0;


WINDOW *create_win(int height, int width, int starty, int startx) {
  WINDOW *local_win = newwin(height, width, starty, startx);
  box(local_win, 0, 0);
  wrefresh(local_win);

  return (local_win);
}

void place_in_editor(WINDOW *win, int x, int y, char c) {
  /*if (c == '\n')
    return;*/

  x++;
  y++;
  mvwprintw(win, x, y, "%c", c);
}

void print_content(WINDOW *win, char content[MAX_LINES][MAX_COLUMNS]) {
  editor.num_chars = -1;
  for (int i = 0; i < MAX_LINES; i++) {
    for (int j = 0; j < MAX_COLUMNS; j++) {
      if (content[i][j] != NULL) {
        place_in_editor(win, i, j, content[i][j]);
        editor.num_chars++;
      }
    }
  }
}

void recovery_array(WINDOW *win, char array[MAX_COLUMNS],
                    char content[MAX_LINES][MAX_COLUMNS], int y, int x) {
  x--;
  y--;
  for (int i = 0; i < MAX_COLUMNS; i++) {
    if (array[i] == NULL) {
      array[i] = ' '; // procurar pelo código do espaço
    }
    content[y][i] = array[i];
    place_in_editor(win, y, i, content[y][i]);
  }
}

void delete_char(WINDOW *win, char content[MAX_LINES][MAX_COLUMNS], int x,
                 int y) {
  x--;
  y--;

  if (content[y][x] == '\0')
    return;
  else {
    for (int i = 0; i < MAX_COLUMNS; i++) {
      if (i >= x) {
        if (content[y][i + 1] == '\n' || content[y][i + 1] == NULL || i == 44) {
          content[y][i] = ' ';
        } else
          content[y][i] = content[y][i + 1];
      }
      place_in_editor(win, y, i, content[y][i]);
    }
    editor.num_chars--;
  }
}

void add_char(WINDOW *win, char content[MAX_LINES][MAX_COLUMNS], char c, int x,
              int y) {
  int i;
  x--;
  y--;
  i = x; // guardar pos x, da coluna

  if (content[y][MAX_COLUMNS - 1] == ' ' ||
      content[y][MAX_COLUMNS - 1] ==
          '\0' || // verifica se a ultima coluna tem espaco enter ou null
      content[y][MAX_COLUMNS - 1] == NULL) {
    for (x = MAX_COLUMNS - 1; x != i;
         x--) { // comeca a percorrer o array da ultima coluna ate a pos onde
                // queremos meter o ch
      if (content[y][x] == NULL) {
        content[y][x] = ' ';
      }
      if (content[y][x - 1] == NULL) {
        content[y][x] = ' ';
        place_in_editor(win, y, x, content[y][x]);
      }
      if (content[y][x - 1] != NULL) {
        content[y][x] = content[y][x - 1]; // arrastar caracteres para a direita
        place_in_editor(win, y, x, content[y][x]);
      }
    }
    content[y][x] = c; // colocar o caracter que queremos no sitio certo
    place_in_editor(win, y, x, content[y][x]);
    editor.num_chars++;
  } else
    return;
}

void forced_shutdown() {
  char pipe[20];
  sprintf(pipe, "../pipe-%d", getpid());
  unlink(pipe);
  clear();
  printw("Programa terminado!\nPressione qualquer tecla para sair");
  refresh();
  getch();
  endwin();
  exit(0);
}

void shutdown(){
  char pipe[20];
  int fd;
  aux send;
  if(logged == 1){
    send.pid = getpid();
    send.action = CLIENT_SHUTDOWN;
    sprintf(pipe, "../pipe-%d", getpid());
    fd = open(PIPE, O_WRONLY, 0600);
    write(fd, &send, sizeof(send));
    unlink(pipe);
    clear();
    printw("Programa encerrou!\nPressione qualquer tecla para sair");
    refresh();
    getch();
    endwin();
    exit(0);
  }else{
    forced_shutdown();
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
      forced_shutdown();
      break;
    case LOGGED: // LOGIN DO CLIENTE COM SUCESSO
      logged = 1;
      read(fd_pipe, &editor, sizeof(editor));
      break;
    case NOT_LOGGED: // USERNAME NAO ENCONTRADO NA BASE DE DADOS
      printf("Username invalido\n");
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

int main(int argc, char **argv) {
  aux temp;
  char pipe[20], npipe[20];
  int opt, fd, res;
  pthread_t task;
  // fd, file handler para lidar com o pipe
  // res , para criar a thread do cliente
  signal(SIGINT, SIGhandler);

  // vou buscar o nome de utilizador do cliente
  while ((opt = getopt(argc, argv, "u:p:")) != -1) {
    switch (opt) {
    case 'u':
      if (optarg) // vai analisar se -u foi introduzido pelo user e guarda-o
        strcpy(temp.user, optarg); // FALTA VERIFICAR LOGIN AQUI !!!!
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
    scanf("%9s", temp.user);
    temp.action = LOGIN; // flag LOGIN para o servidor saber o que fazer
    write(fd, &temp, sizeof(temp));
    sleep(1);
  } while (logged == 0);

  if (logged == 1) {
    int ch;
    int x = 1;
    int y = 1;

    WINDOW *my_win;
    WINDOW *info;

    initscr();
    cbreak();
    keypad(stdscr, TRUE); // para ativar a leitura das setas
    noecho();

    printw("Bem Vindo: %9s\tPress Esc to exit", temp.user);
    refresh();

    my_win = create_win(HEIGHT, WIDTH, y, x);
    info = create_win(3, WIDTH, HEIGHT + 1, 1);

    wmove(my_win, y, x);

    print_content(my_win, editor.content);

    mvwprintw(info, 1, 1, "Chars : ");
    mvwprintw(info, 1, 9, "%d", editor.num_chars);

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
        s[i] = editor.content[y - 1][i];
      }
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
        mvprintw(y + 1, 58, "%s", temp.user);
        refresh();
        wmove(my_win, y, x); // Start with cursor in 1 1
        wrefresh(my_win);
        editor.client.status = true;
        editor.client.editing_line = y;
        write(fd, &editor, sizeof(editor));

        while ((ch = getch()) != 10) {

          if (ch == 27) {
            recovery_array(my_win, s, editor.content, y, x);
            mvprintw(y + 1, 58, "        ");
            refresh();
            editor.client.status = false;
            write(fd, &editor, sizeof(editor));
            break;
          }

          switch (ch) {
          case KEY_DC:        // delete
          case KEY_BACKSPACE: // backspace
          case 8:             // delete
          case 127:           // backspace
            delete_char(my_win, editor.content, x, y);
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
            add_char(my_win, editor.content, ch, x, y);
            if (x < 45) {
              x++;
            }
          }
          wmove(my_win, y, x);
          mvwprintw(info, 1, 9, "%d", editor.num_chars);
          wrefresh(info);
          wrefresh(my_win);
        }
      }
      mvprintw(y + 1, 58, "        ");
      refresh();
      wmove(my_win, y, x);
      mvwprintw(info, 1, 9, "%d", editor.num_chars);
      wrefresh(info);
      wrefresh(my_win);
      write(fd, &editor, sizeof(editor));
    }
    endwin();
  }

  pthread_join(task, NULL);
  close(fd);
  unlink(pipe);
  return 0;
}
