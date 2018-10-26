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
  int lines, columns;
  Cursor cursor;
  char content[MAX_LINES][MAX_COLUMNS];
  int screenrows;
  int num_chars;
  int timeout;
  int max_users;
} Editor;

typedef struct{
  char username[8];
  int pid;
  bool status; //modo navegação ou edição
}user;