#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <stdbool.h>
#define MAX_LINES 15
#define MAX_COLUMNS 45
#define MEDIT_TIMEOUT 10
#define MEDIT_MAXUSERS 3
#define PIPE "../main_pipe"

#define SERVER_SHUTDOWN 1
#define CLIENT_SHUTDOWN 2
#define LOGIN 3
#define LOGGED 4
#define NOT_LOGGED 5
#define MAX_ACTIVE_USERS 6

typedef struct {
  int x, y;
} Cursor;

typedef struct {
  char user[8];
  int pid, action;
  // action = variavel para ajudar em acções como o SHUTDOWN, ente outras
} aux;

typedef struct {
  int lines, columns;
  char content[MAX_LINES][MAX_COLUMNS];
  int num_chars;
  int timeout;
  int max_users;
  Cursor cursor;
  char username[8];
  int pid;
  int action;
  bool status;      // modo navegação ou edição
  int editing_line; // para saber qual a linha que está a editar no momento
  int n_chars;
} Editor;

#endif
