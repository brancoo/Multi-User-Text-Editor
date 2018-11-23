#include "estruturas.h"
#include <ncurses.h>

WINDOW *create_win(int height, int width, int starty, int startx);
void place_in_editor(WINDOW *win, int x, int y, char c);
void print_content(WINDOW *win, char content[MAX_LINES][MAX_COLUMNS]);
void recovery_array(WINDOW *win, char array[MAX_COLUMNS],
                    char content[MAX_LINES][MAX_COLUMNS], int y, int x);
void delete_char(WINDOW *win, char content[MAX_LINES][MAX_COLUMNS], int x,
                 int y);
void add_char(WINDOW *win, char content[MAX_LINES][MAX_COLUMNS], char c, int x,
              int y);
