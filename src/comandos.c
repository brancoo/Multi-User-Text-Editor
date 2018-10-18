#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int cmd(char *com)
{
    char *arg[3];
    char user[25], pass[10];
    int i, flag = 0;

    FILE *f;

    f = fopen("utilizadores.txt", "a+");
    if (!f)
    {
        printf("ERRO! Ao abrir o ficheiro!\n");
        return 0;
    }

    *arg = strtok(com, " ");

    if (strcmp(*arg, "shutdown") == 0)
    {
        exit(0);
    }
    
    if(strcmp(*arg, "users") == 0)
        return 0;
   
    if(strcmp(*arg, "settings") == 0)
        return 0;
    
    return 1;
    
}