#define MEDIT_MAXUSERS 3 // Nº de utilizadores máximo ao mesmo tempo
#define MAX_LINES 15
#define MAX_COLUMNS 45

// enum editorKey {   //TECLAS DE MANIPULAÇÃO (PARA O CLIENTE)
//   ARROW_LEFT = 1000,
//   ARROW_RIGHT,      //fica automaticamente com o valor incrementado ao valor
//   anterior (=1001) ARROW_UP,         //(=1002) ARROW_DOWN,       //(=1003)
//   BACKSPACE = 127,
//   ESC,              //(=128)
//   ENTER,            //(=129)
//   DEL_KEY           //(=130)
// };

// typedef struct{
//     char username[50];
//     int pid;
// }user;

// typedef struct{
//     int size; //tamanho da linha (em colunas) em questão
//     char *chars; //conteúdo da linha
// }editor_row;

// typedef struct{
//     int cx, cy;             //coordenadas X e Y, para navegação no
//     editor(cursor) int num_rows;           //nº de linhas do editor de texto
//     (por omissão, MEDIT_MAXLINES) int num_col;            //nº de colunas do
//     editor (por omissão, MEDIT_MAXCOLUMNS) int words_total;        //contador
//     para número de palavras int letters;            //contador para número de
//     letras char *filename;         //nome do ficheiro a ser editado bool
//     modified;          //para saber se o ficheiro carregado foi modificado ou
//     não editor_row *row;        //acesso a uma determinada linha
// } settings;

typedef struct {
  int x, y;
} Cursor;

typedef struct {
  int lines, columns;
  Cursor cursor;
  char *filename;
  char content[MAX_LINES][MAX_COLUMNS];
  int screenrows;
  int size;
  int num_chars;
} Editor;