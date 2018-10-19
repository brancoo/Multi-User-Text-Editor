#define MEDIT_MAXUSERS 3 // Nº de utilizadores máximo ao mesmo tempo
#define MAX_LINES 15
#define MAX_COLUMNS 45



typedef struct {
  int x, y;
} Cursor;

typedef struct {
  int lines, columns;
  Cursor cursor;
  char *filename;
  char content[MAX_LINES][MAX_COLUMNS];
  int screenrows;
  int size;
  int num_chars;
} Editor;