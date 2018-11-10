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
aux temp;

void load_file(char *filename) {
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    printf("Erro ao carregar ficheiro : %s\n", filename);
    return;
  }

  for (int x = 0; x < MAX_LINES; x++) {
    for (int y = 0; y < MAX_COLUMNS; y++) {
      fscanf(file, "%c", &editor.content[x][y]);
    }
  }

  fclose(file);
}

WINDOW *create_win(int height, int width, int starty, int startx) {
  WINDOW *local_win = newwin(height, width, starty, startx);
  box(local_win, 0, 0);
  wrefresh(local_win);

  return (local_win);
}

void place_in_editor(WINDOW *win, int x, int y, char c) {
  if (c == '\n')
    return;

  x++;
  y++;
  mvwprintw(win, x, y, "%c", c);
}

void print_content(WINDOW *win, char content[MAX_LINES][MAX_COLUMNS]) {
  for (int i = 0; i < MAX_LINES; i++) {
    for (int j = 0; j < MAX_COLUMNS; j++) {
      if (content[i][j] != NULL) {
        place_in_editor(win, i, j, content[i][j]);
        editor.num_chars++;
      }
    }
  }
}

void edit_array(WINDOW *win, char content[MAX_LINES][MAX_COLUMNS], int x,
                int y) {
  x--;
  y--;

  if (content[y][x] == '\0')
    return;
  else {
    for (int i = 0; i < MAX_COLUMNS; i++) {
      if (i >= x) {
        if (content[y][i + 1] == '\n' || i == 44) {
          content[y][i] = ' ';
        } else
          content[y][i] = content[y][i + 1];
      }
      place_in_editor(win, y, i, content[y][i]);
    }
    editor.num_chars--;
  }
}

void edit_editor(WINDOW *win, char content[MAX_LINES][MAX_COLUMNS], char c,
                 int x, int y) {
  int i, s;
  x--;
  y--;
  s = strlen(content[y]);
  i = x;

  if (s != 0) {
    if (s - 1 < MAX_COLUMNS) {
      if (s - 1 > x) {
        for (x = s; x != i; x--) {
          if (content[y][x] == NULL) {
            content[y][x] = ' ';
          }
          content[y][x] = content[y][x - 1];
          place_in_editor(win, y, x, content[y][x]);
        }
        content[y][x] = c;
        place_in_editor(win, y, x, content[y][x]);
        editor.num_chars++;
      } else {
        content[y][x] = c;
        place_in_editor(win, y, x, content[y][x]);
        editor.num_chars++;
      }
    } else
      return;
  } else {
    content[y][x] = c;
    place_in_editor(win, y, x, content[y][x]);
    editor.num_chars++;
  }
}

void verify() {
  int fd;
  char pipe[20];

  sprintf(pipe, "../pipe-%d", getpid());
  fd = open(pipe, O_RDONLY);
  read(fd, &temp, sizeof(temp)); // está a ler endereços de memória
  if (temp.action == 1) {
    printf("Programa terminado!\n");
    unlink(pipe);
    exit(0);
  } else {
    return;
  }
}
void shutdown() {
  char pipe[20];
  sprintf(pipe, "pipe-%d", getpid());
  unlink(pipe);
  clear();
  printw("Programa terminado!\n");
  refresh();
  getch();
  endwin();
  exit(0);
}
void SIGhandler(int sig) {
  signal(sig, SIG_IGN);
  shutdown();
}

int main(int argc, char **argv) {
  char pipe[20], npipe[20];
  int opt, fd, fd_client;
  // fd, file handler para lidar com o pipe

  signal(SIGINT, SIGhandler);
  // vou buscar o nome de utilizador do cliente
  while ((opt = getopt(argc, argv, "u:p:")) != -1) {
    switch (opt) {
    case 'u':
      if (optarg) // vai analisar se -u foi introduzido pelo user e guarda-o
        strcpy(temp.user, optarg);
      break;
    case 'p':
      if (optarg) // vai analisar se -p foi introduzido pelo user
        strcpy(pipe,
               optarg); // copia o valor do argumento para a variável pipe
      else { // senão existir argumento opcional, toma o valor por omissão
        strcpy(pipe, PIPE);
      }
      break;
    }
  }

  if (strlen(temp.user) == 0) {
    printf("Username: "); // senão existir então é pedido explicitamente
    scanf("%s", temp.user);
  }

  if (access(PIPE, F_OK) != 0) {
    printf("O servidor nao se encontra em execucao. A sair...\n");
    getch();
    exit(0);
  }
  temp.pid = getpid();
  sprintf(npipe, "../pipe-%d", temp.pid);
  if (mkfifo(npipe, S_IRWXU) == -1) {
    printf("Erro ao criar pipe. A sair...\n");
    getch();
    exit(0);
  }

  fd = open(PIPE, O_RDWR); // abrir para leitura/escrita
  if (fd < 0) {
    printf("Erro a abrir o pipe do servidor. A sair...\n");
    getch();
    exit(0);
  }

  char buffer[30];
  write(fd, &temp, sizeof(temp));
  fd_client = open(npipe, O_RDONLY);
  read(fd_client, buffer, sizeof(buffer));
  printf("%s\n", buffer); // VAI DIZER SE O USER ESTÁ VERIFICADO OU NÃO
  // verify();

  load_file("../out/text.txt");

  int ch;
  int x = 1;
  int y = 1;

  WINDOW *my_win;
  WINDOW *info;

  initscr();
  cbreak();
  keypad(stdscr, TRUE); // para ativar a leitura das setas
  noecho();

  printw("Bem Vindo: %s\tPress Esc to exit", temp.user);
  refresh();

  my_win = create_win(HEIGHT, WIDTH, y, x);
  info = create_win(3, WIDTH, HEIGHT + 1, 1);

  wmove(my_win, y, x);

  print_content(my_win, editor.content);

  mvwprintw(info, 1, 1, "Chars : ");
  mvwprintw(info, 1, 9, "%d", editor.num_chars);

  wrefresh(info);

  // Start with cursor in 1 1
  wmove(my_win, 1, 1); // meter cursor na pos 1,1
  wrefresh(my_win);

  while ((ch = getch()) != 27) // sai ciclo quando clicar escape
  {
    switch (ch) {
    case KEY_DC:        // delete
    case KEY_BACKSPACE: // backspace
    case 8:             // delete
    case 127:           // backspace

      edit_array(my_win, editor.content, x, y);

      break;
    case 10:
      if (y < 15) {
        y += 1;
        x = 1;
      }
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
    default:
      edit_editor(my_win, editor.content, ch, x, y);
      if (x == 45 && y < 15) {
        y++;
        x = 0;
      }
      if (x != 45 && y <= 15) {
        x++;
      }
    }

    wmove(my_win, y, x);
    mvwprintw(info, 1, 9, "%d", editor.num_chars);
    wrefresh(info);
    wrefresh(my_win);
  }
  endwin();
  close(fd);
  unlink(pipe);
  return 0;
}