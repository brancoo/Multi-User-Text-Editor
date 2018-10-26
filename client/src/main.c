#include "estruturas.h"
//#include <curses.h>
#include <getopt.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WIDTH 47
#define HEIGHT 17

Editor editor;

void load_file(char *filename) {
  FILE *file = fopen(filename, "rt");

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

void init_editor() {
  editor.cursor.x = 4;
  editor.cursor.y = 11;
  editor.lines = 15;
  editor.columns = 45;
  editor.screenrows = 0;
  editor.num_chars = 0;
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

  if(content[y][x]=='\0') 
  return;
  else{
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
  x--;
  y--;

  if (c == '\n') {
    return;
  }

  if(content[y][x]=='\0'){
    editor.num_chars++;
  }

  content[y][x] = c;
  place_in_editor(win, y, x, c);
}

int main(int argc, char **argv) {
  char user[8], pipe[10];
  int opt;

  // vou buscar o nome de utilizador do cliente
  while ((opt = getopt(argc, argv, "u:p:")) != -1) {
    switch (opt) {
    case 'u':               // vai analisar se -u foi introduzido pelo user
      strcpy(user, optarg); // se houver argumento copia-o para a variável user
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

  if (strlen(user) == 0) {
      printf("Username: "); // senão existir então é pedido explicitamente
      scanf("%s", user);
  }

  init_editor();
  load_file("out/text.txt");

  int ch;
  int x = 1;
  int y = 1;

  WINDOW *my_win;
  WINDOW *info;

  initscr();
  cbreak();
  keypad(stdscr, TRUE); // para ativar a leitura das setas
  noecho();

  printw("Bem Vindo: %s\tPress Esc to exit", user);
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
  return 0;
}
