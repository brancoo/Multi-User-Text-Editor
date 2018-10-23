#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include "main.h"

Editor editor;
int max_users;

bool verify_file_existence(char *file){
    FILE *f = fopen(file, "r");

    if(f == NULL){
        return 1;
    }else{
        return 0;
    }
}

int getMAX_USERS(int n){  //vai ser preciso para o MAX_USERS
    srand(time(NULL));
    return rand() % n;
}

bool find_username(char *username, char *filename){
	FILE *f = fopen(filename, "rt");
	char c, buffer[50];

	if (f == NULL){
		printf("ERRO AO ABRIR FICHEIRO!\n");
		return false;
	}

	while((c = getc(f)) != EOF){
		fscanf(f,"%s",buffer);
		if (strcmp(buffer, username) == 0){
			return true;
            fclose(f);
        }
	}
	fclose(f);
	return false;
}

void verify_env_var(){
    //se a variável de ambiente não existe, então assumimos o valor por omissão (=15 linhas)
    if (getenv("MEDIT_MAXLINES") == NULL)
        editor.lines = MAX_LINES;
    else{
        editor.lines = atoi(getenv("MEDIT_MAXLINES"));
    }
    //se a variável de ambiente não existe, então assumimos o valor por omissão (=45 colunas)
    if(getenv("MEDIT_MAXCOLUMNS") == NULL)
        editor.columns = MAX_COLUMNS;
    else{
        editor.columns = atoi(getenv("MEDIT_MAXCOLUMNS"));
    }

    if(getenv("MEDIT_TIMEOUT") == NULL)
        editor.timeout = MEDIT_TIMEOUT;
    else{
        editor.timeout = atoi(getenv("MEDIT_TIMEOUT"));
    }
    if(getenv("MEDIT_MAXUSERS") == NULL)
        max_users = MEDIT_MAXUSERS;
    else{
        max_users = getMAX_USERS(editor.lines);
    }
}

int main(int argc, char *argv[])
{
    char *file;
    int opt;
    
    //saber se o admin enviou pela linha de comandos
    if((opt = getopt(argc, argv, "-f")) != -1){
        if (verify_file_existence(file) == 0) //verificar se o ficheiro dado pelo admin existe
            file = argv[2];  //guardar o nome da base de dados que contém os usernames
        else{
            printf("Ficheiro nao existe!\n");
        }
    }
    else{
        file = "medit.db";
        printf("Base de dados assumida por defeito: %s\n",file);
    }
        verify_env_var();
        return 0;
}
