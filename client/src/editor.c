#include "editor.h"
#include "main.h"

WINDOW *create_win(int height, int width, int starty, int startx) {
  WINDOW *local_win = newwin(height, width, starty, startx);
  box(local_win, 0, 0);
  wrefresh(local_win);

  return (local_win);
}

void place_in_editor(WINDOW *win, int x, int y, char c) {
  x++; // coordenadas do cursor do editor
  y++;
  mvwprintw(win, x, y, "%c", c);
}

void print_content(WINDOW *win, char content[MAX_LINES][MAX_COLUMNS]) {
  receive.num_chars = -1;
  for (int i = 0; i < MAX_LINES; i++) {
    for (int j = 0; j < MAX_COLUMNS; j++) {
      if (content[i][j] != NULL) {
        place_in_editor(win, i, j, content[i][j]);
        receive.num_chars++;
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

  if (content[y][x] == ' ') // estava '\0'
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
    receive.num_chars--;
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
    receive.num_chars++;
  } else
    return;
}