#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "structs.h"

bool find_username(char *username, char *filename){
	FILE *f = fopen(filename, "rt");
	char c, buffer[50];

	if (f == NULL){
		printf("ERRO AO ABRIR FICHEIRO!\n");
		return false;
	}

	while((c = getc(f)) != EOF){
		fscanf(f,"%s",buffer);
		if (strcmp(buffer, username) == 0)
			return true;
	}
	fclose(f);
	return false;
}

void keyboard(char *cmd){
	char **arg = NULL;
  	char *p = strtok(cmd, " ");
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

	if (strcmp(arg[0], "load") == 0){
		if(strcmp(arg[1],NULL) != 0) //se existir nome do ficheiro | FALTA MOSTRA MENSAGEM DE ERRO
			load_settings(arg[1]);
	}
	else if (strcmp(arg[0], "save") == 0){
		if (strcmp(arg[1], NULL) != 0)
			save_settings(arg[1]);  //guardar definições em ficheiro .txt
	}
	else if (strcmp(arg[0], "free") == 0){
		if (strcmp(arg[1], NULL) != 0)
			free_row(arg[1]);	//libertar/apagar o conteúdo de determinada linha
	}
	else if (strcmp(arg[0], "statistics") == 0)
		statistics();
	else if (strcmp(arg[0], "users") == 0)
		users();
	else if (strcmp(arg[0], "text") == 0)
		text();
	else if (strcmp(arg[0], "shutdown") == 0)
		shutdown();
	else{
		 printf("Comando inválido!\n");
	}

	free(arg);
}