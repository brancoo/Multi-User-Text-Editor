#include "../include/estruturas.h"
#include <curses.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WIDTH 47 //PARA DESENHAR O EDITOR
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
  editor.filename = NULL;
  editor.num_chars = 0;
}

WINDOW *create_win(int height, int width, int starty, int startx) {
  WINDOW *local_win = newwin(height, width, starty, startx);
  box(local_win, 0, 0);
  wrefresh(local_win);

  return (local_win);
}

void place_in_editor(WINDOW *win, int x, int y, char c) {
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

void edit_editor(WINDOW *win, char content[MAX_LINES][MAX_COLUMNS], char c, int x, int y){
  x--;
  y--;
  content[x][y] = c;
  place_in_editor(win, y, x, c);
}

int main(int argc, char **argv) {
  
  init_editor();
  load_file("out/text.txt");

  int ch;
  int x = 1;
  int y = 1;

  WINDOW *my_win;
  WINDOW *info;

  initscr();
  cbreak();
  keypad(stdscr, TRUE); //para ativar a leitura das setas
  noecho();

  printw("Press q to exit");
  refresh();

  my_win = create_win(HEIGHT, WIDTH, y, x);
  info = create_win(3, WIDTH, HEIGHT + 1, 1);

  wmove(my_win, y, x);

  print_content(my_win, editor.content);

  mvwprintw(info, 1, 1, "Chars : ");
  mvwprintw(info, 1, 9, "%d", editor.num_chars);

  wrefresh(info);

  // Start with cursor in 1 1
  wmove(my_win, 1, 1);  //meter cursor no inicio da 1 janela
  wrefresh(my_win);

  while ((ch = getch()) != 'q') {
    switch (ch) {
    case KEY_LEFT:
      if (x > 1) {
        x--;
      }
      break;
    case KEY_RIGHT:
      if (x < 45) {
        x++;
      }
      break;
    case KEY_UP:
      if (y > 1) {
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
    if(x == 45){
      x=1;
      y++;
    }
    else
    x++;
    }
    wmove(my_win, y, x);
    wrefresh(my_win);
  }

  endwin();
  return 0;
}
