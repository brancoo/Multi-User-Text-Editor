#include <stdbool.h>
#define MAX_LINES 15
#define MAX_COLUMNS 45
#define MEDIT_TIMEOUT 10
#define MEDIT_MAXUSERS 3
#define PIPE "main_pipe"

typedef struct {
  int x, y;
} Cursor;

typedef struct {
  char username[8];
  int pid;
  bool status;      // modo navegação ou edição
  int editing_line; // para saber qual a linha que está a editar no momento
} user;

typedef struct {
  int lines, columns;
  Cursor cursor;
  char content[MAX_LINES][MAX_COLUMNS];
  int screenrows;
  int num_chars;
  int timeout;
  int max_users;
  user client;
} Editor;
