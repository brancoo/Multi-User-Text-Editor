#define MEDIT_MAXUSERS 3 //Nº de utilizadores máximo ao mesmo tempo
#define MEDIT_MAXLINES 15  //Nº de linhas por omissão
#define MEDIT_MAXCOLUMNS 45  //Nº de colunas por omisão



typedef struct{
    char username[50];
    int pid;
}user;

typedef struct{
    int size; //tamanho da linha em questão
    char *chars; //conteúdo da linha
}editor_row;
                  
typedef struct{
    char matrix[MEDIT_MAXLINES][MEDIT_MAXCOLUMNS];    //matriz de caracteres
    int words_total;        //contador para número de palavras
    int letters;            //contador para número de letras
    char *filename;         //nome do ficheiro .txt a ser editado
    bool modified;          //para saber se o ficheiro .txt carregado foi modificado ou não
}editor;

typedef struct{
    int cx, cy; //coordenadas X e Y, para navegação no editor(cursor)
    user _user;
    editor _editor;
    editor_row *row;
    bool status; //para saber se está em modo navegação ou edição
}user_active;