#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "structs.h"

void mostrar()		//temos que ir buscar o nome do ficheiro que está a ser editado e mostrá-lo
{ 			
	FILE *f;
	char c[8];

	f = fopen("users.txt", "r");
	if (f){
		while (fscanf(f, "%s", c) != EOF)
			printf("%.8s\n", c);
	}
	fclose(f);
	return;
}

void escrever_ficheiro(char *user)
{
	FILE *f = fopen("users.txt", "at");
	fprintf(f, "\n%s", user);
	fclose(f);
	return;
}

int main(int argc, char *argv[])
{
	mostrar();
	escrever_ficheiro("joao");
	mostrar();
	return 0;
}