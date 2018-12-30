#include "comandos.h"
#include "main.h"
#include "users.h"
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/unistd.h>

bool verify_file_existence(char *file) {
  if (access(file, F_OK) != -1) {
    return true; // SE EXISTIR
  } else {
    return false; // SE NÃO EXISTIR
  }
}

// INICIALIZA A MATRIZ DO EDITOR COM ESPAÇOS EM BRANCO
void initialize_editor_content() {
  for (int i = 0; i < editor.lines; i++) {
    for (int j = 0; j < editor.columns; j++)
      editor.content[i][j] = ' ';
  }
}

void shutdown() {
  char pipe[20];
  int fd;
  Editor send;
  send.action = SERVER_SHUTDOWN;

  for (int i = 0; i < active_users; i++) {
    sprintf(pipe, "../pipe-%d", clients[i].pid);
    fd = open(pipe, O_WRONLY, 0600);
    write(fd, &send, sizeof(send));
    close(fd);
  }

  unlink(PIPE);
  printf("Programa terminado\n");
  exit(0);
}

void save_settings(char *filename) {
  FILE *f = fopen(filename, "w");
  int i, j;

  if (f == NULL) {
    printf("Erro com o ficheiro %s\n", filename);
    fclose(f);
    return;
  } else {
    for (i = 0; i < editor.lines; i++) {
      for (j = 0; j < editor.columns; j++) {
        fprintf(f, "%c", editor.content[i][j]);
      }
    }
  }
  fclose(f);
}

void users() {
  time_t rawtime;
  struct tm *loc_time;
  int m, s;
  char pipe[20];

  system("clear");

  if (active_users > 0) {
    for (int i = 0; i < active_users; i++) {
      sprintf(pipe, "pipe-%d", clients[i].pid);
      printf("\nUtilizador: %s\n", clients[i].username);
      printf("Nome do Pipe: %s\n", pipe);
      rawtime = time(NULL);
      loc_time = localtime(&rawtime);
      m = loc_time->tm_min;
      s = loc_time->tm_sec;
      printf("Idade da Sessao: %dminutos:%dsegundos\n",
             m - clients[i].user_time.minutos,
             s - clients[i].user_time.segundos);
      if (clients[i].status == true) {
        printf("A editar a linha %d\n", clients[i].editing_line);
      }
    }
  } else {
    printf("Nenhum user com sessao iniciada!\n");
  }
}

void settings() {
  char *aux;
  aux = strtok(PIPE, "../");
  system("clear");

  printf("Numero de Linhas: %d\nNumero de colunas: %d\nNumero Max. de "
         "Utilizadores: %d\nNome do Pipe Principal: %s\n",
         editor.lines, editor.columns, editor.max_users, aux);
}

void load_file(char *filename) {
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    printf("Erro ao carregar ficheiro : %s\n", filename);
    return;
  }

  for (int x = 0; x < editor.lines; x++) {
    for (int y = 0; y < editor.columns; y++) {
      fscanf(file, "%c", &editor.content[x][y]);
    }
  }
  fclose(file);
}

void text() {
  system("clear");

  for (int i = 0; i < editor.lines; i++) {
    printf("%d ", i + 1); // FALTAR IMPRIMIR O NOME DO USER
    for (int j = 0; j < editor.columns; j++) {
      printf("%c", editor.content[i][j]);
    }
    printf("\n");
  }
}

void free_row(int line) {
  for (int i = 0; i < MAX_COLUMNS; i++)
    if (editor.content[line][i] != ' ') {
      editor.content[line][i] = ' ';
      editor.num_chars--;
    }
}

void count_chars(char *aux) {
  int count = 0;
  while (*aux) {
    if (!isspace(*aux++))
      count++;
  }
  printf("Numero de letras:%d\n", count);
}

void statistics() {
  // cria-se uma cópia auxiliar da nossa matriz para não se mexer na matriz
  // original
  char *aux = (char *)malloc(editor.lines * editor.columns * sizeof(char));
  for (int i = 0; i < editor.lines; i++) {
    for (int j = 0; j < editor.columns; j++) {
      *(aux + i * editor.columns + j) = editor.content[i][j];
    }
  }

  char *p = strtok(aux, " ,.!?-'");
  int n_words = 0;

  while (p) {
    printf("Palavra: %s\n", p);
    count_chars(p);
    ++n_words;
    p = strtok(NULL, " ");
  }
  printf("\nTotal de Palavras:%d\n", n_words);

  for (int i = 0; i < active_users; i++) {
    if (clients[i].status == true) {
      printf("Linha %d esta a ser editada pelo cliente %s.\n",
             clients[i].editing_line, clients[i].username);
    }
  }

  free(p);
  free(aux);
}

void verify_word() {
  // atos -> aspell to server
  // stoa -> server to aspell
  int atos[2], stoa[2], pid;
  pipe(atos);
  pipe(stoa);
  pid = fork();
  if (pid == 0) {
    close(0);
    dup(stoa[0]);
    close(1);
    dup(atos[1]);
    close(stoa[0]);
    close(stoa[1]);
    close(atos[0]);
    close(atos[1]);
    execlp("aspell", "aspell", "-a", "pt_PT", NULL);
  }
  char asp_char;

  // para "limpar" a mensagem inicial do aspell
  do {
    read(atos[0], &asp_char, sizeof(asp_char));
  } while (asp_char != '\n');

  char palavra[50];
  printf("Palavra a verificar: ");
  scanf("%s", palavra);
  strcat(palavra, "\n");

  // envia a palavra para o aspell
  write(stoa[1], palavra, sizeof(palavra));
  // aspel retorna um caracter(* -> CERTO || & -> ERRADO)
  read(atos[0], &asp_char, sizeof(asp_char));

  if (asp_char == '*') {
    printf("CERTO\n");
  } else {
    printf("ERRADO\n");
  }
}

void cmd(char *com) {
  char **arg = NULL;
  char *p = strtok(com, " ");
  int n_spaces = 0;

  /* dividir a string e anexar tokens em 'arg' */
  while (p) {
    arg = realloc(arg, sizeof(char *) * ++n_spaces);
    if (arg == NULL)
      exit(-1); /* falha na alocação de meḿória */
    arg[n_spaces - 1] = p;
    p = strtok(NULL, " ");
  }

  /* Realocar um elemento extra para o último NULL */
  arg = realloc(arg, sizeof(char *) * (n_spaces + 1));
  arg[n_spaces] = 0;

  if (strcmp(arg[0], "load") == 0) {
    if (arg[1]) {
      if (verify_file_existence(arg[1]) == true)
        load_file(arg[1]); // load ../out/exemplo.txt  (EXEMPLO!)
      else {
        printf("Ficheiro nao encontrado!\n");
        return;
      }
    } else {
      printf("Faltam argumentos!\n");
      return;
    }
  } else if (strcmp(arg[0], "save") == 0) {
    if (arg[1])
      save_settings(arg[1]); // ../out/exemplo.txt (exemplo)
    else {
      printf("Faltam argumentos!\n");
      return;
    }
  } else if (strcmp(arg[0], "shutdown") == 0)
    shutdown();
  else if (strcmp(arg[0], "settings") == 0)
    settings();
  else if (strcmp(arg[0], "sair") == 0)
    shutdown();
  else if (strcmp(arg[0], "users") == 0)
    users();
  else if (strcmp(arg[0], "text") == 0)
    text();
  else if (strcmp(arg[0], "free") == 0) {
    if (arg[1]) {
      free_row(atoi(arg[1]));
      editor.action = UPDATE;
      update_all_users();
      update_Free_Line_User(atoi(arg[1]));
    } else {
      printf("Faltam argumentos!\n");
      return;
    }
  } else if (strcmp(arg[0], "statistics") == 0)
    statistics();
  else if (strcmp(arg[0], "aspell") == 0)
    verify_word();
  else {
    printf("Comando inválido!\n");
    return;
  }
  free(arg);
}