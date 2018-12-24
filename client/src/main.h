#include "estruturas.h"
#include <ncurses.h>

#define WIDTH 47
#define HEIGHT 17

extern WINDOW *my_win;
extern WINDOW *info;
extern Editor receive;
extern int logged;
extern int stop;
extern int x, y;
extern int permiAccepted;

void SIGhandler(int sig);
void alarme(int sig);
int main(int argc, char **argv);
void *receiver();
void shutdown();
void server_shutdown();
void client_shutdown();