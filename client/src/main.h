#include "estruturas.h"

#define WIDTH 47
#define HEIGHT 17

aux receive;
Editor editor;
extern int logged;

void SIGhandler(int sig);
int main(int argc, char **argv);
void *receiver();
void shutdown();
void server_shutdown();
void client_shutdown();