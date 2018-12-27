#include "users.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

bool find_username(char *username, char *filename) {
  FILE *f = fopen(filename, "r");
  char buffer[9];

  if (f == NULL) {
    printf("ERRO AO ABRIR FICHEIRO!\n");
    fclose(f);
    return false;
  }

  while (fscanf(f, "%8s", buffer) == 1) {
    if (strcmp(buffer, username) == 0) {
      fclose(f);
      return true;
    }
  }
  fclose(f);
  return false;
}

bool check_if_users_exceeds_max_active() {
  if (active_users >= editor.max_users) {
    return false; // ATINGIDO O LIMITE MÁXIMO DE USERS
  } else {
    return true; // PODE FAZER LOGIN
  }
}

void add_to_active_users_list(int pid, char username[8]) {
  time_t rawtime;
  struct tm *loc_time;
  rawtime = time(NULL);
  loc_time = localtime(&rawtime);

  clients[active_users].pid = pid;
  strcpy(clients[active_users].username, username);
  clients[active_users].status = false;
  clients[active_users].user_time.minutos = loc_time->tm_min;
  clients[active_users].user_time.segundos = loc_time->tm_sec;
  active_users++;
}

bool verify_line_edition(Editor aux) {
  for (int i = 0; i < active_users; i++) {

    if (clients[i].editing_line == aux.editing_line &&
        clients[i].status == true) {
      return false;
    }
  }
  return true;
}

bool check_users_existence(char username[8]) {
  for (int i = 0; i < active_users; i++) {
    if (strcmp(username, clients[i].username) == 0) {
      return false;
    }
  }
  return true;
}

void delete_user_from_array(int pid) {
  for (int i = 0; i < active_users; i++) {
    if (clients[i].pid == pid) {
      for (int j = i; j < active_users; j++) {
        clients[j] = clients[j + 1];
      }
    }
  }
  active_users--;
}

void update_all_other_users(int pid) {
  int fd;
  char pipe[20];
  updateAllUsersEditor();
  for (int i = 0; i < active_users; i++) {
    if (clients[i].pid != pid) {
      sprintf(pipe, "../pipe-%d", clients[i].pid);
      fd = open(pipe, O_WRONLY, 0600);
      clients[i].action = UPDATE;
      write(fd, &clients[i], sizeof(clients[i]));

      close(fd);
    }
  }
}

void update_all_users() {
  int fd;
  char pipe[20];
  updateAllUsersEditor();
  for (int i = 0; i < active_users; i++) {
    sprintf(pipe, "../pipe-%d", clients[i].pid);
    fd = open(pipe, O_WRONLY, 0600);
    clients[i].action = UPDATE;
    write(fd, &clients[i], sizeof(clients[i]));
    close(fd);
  }
}