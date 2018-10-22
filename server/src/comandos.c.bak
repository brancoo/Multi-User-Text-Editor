#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"

void settings(){
    printf("Numero de Linhas: %d\nNumero de colunas: %d\nNome da Base de Dados: %d\nNumero Max. de Utilizadores: %d\nNome do Pipe Principal: %s\n", editor.lines, editor.columns,editor.filename,max_users, PIPE);
}

int cmd(char *com)
{
    char **arg = NULL;
    char *p = strtok(com, " ");
    int n_spaces = 0;

    FILE *f = fopen("../out/text.txt", "a+");
    if (!f)
    {
        printf("ERRO! Ao abrir o ficheiro!\n");
        return 0;
    }

    /* dividir a string e anexar tokens em 'arg' */
    while (p)
    {
        arg = realloc(arg, sizeof(char *) * ++n_spaces);
        if (arg == NULL)
            exit(-1); /* falha na alocação de meḿória */
        arg[n_spaces - 1] = p;
        p = strtok(NULL, " ");
    }

    /* Realocar um elemento extra para o último NULL */
    arg = realloc(arg, sizeof(char *) * (n_spaces + 1));
    arg[n_spaces] = 0;

    if (strcmp(arg[0], "load") == 0)
    {
        if (strcmp(arg[1], NULL) != 0) //se existir nome do ficheiro | FALTA MOSTRA MENSAGEM DE ERRO
            load_settings(arg[1]);
        else
        {
            printf("Faltam argumentos!\n");
        }
    }
    else if (strcmp(arg[0], "save") == 0)
    {
        if (strcmp(arg[1], NULL) != 0)
            save_settings(arg[1]); //guardar definições em ficheiro .txt
        else
        {
            printf("Faltam argumentos!\n");
        }
    }
    else if (strcmp(arg[0], "free") == 0)
    {
        if (strcmp(arg[1], NULL) != 0)
            free_row(arg[1]); //libertar/apagar o conteúdo de determinada linha
        else
        {
            printf("Faltam argumentos!\n");
        }
    }
    else if (strcmp(arg[0], "statistics") == 0)
        statistics();
    else if (strcmp(arg[0], "users") == 0)
        users();
    else if (strcmp(arg[0], "text") == 0)
        text();
    else if (strcmp(arg[0], "shutdown") == 0)
        shutdown();
    else if (strcmp(arg[0], "settings") == 0)
        settings();
    else
    {
        printf("Comando inválido!\n");
    }

    return 1;
}