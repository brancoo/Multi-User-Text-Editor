#include "estruturas.h"

#define WIDTH 47
#define HEIGHT 17

Editor receive;
extern int logged;
extern int stop;

void SIGhandler(int sig);
void alarme(int sig);
int main(int argc, char **argv);
void *receiver();
void shutdown();
void server_shutdown();
void client_shutdown();