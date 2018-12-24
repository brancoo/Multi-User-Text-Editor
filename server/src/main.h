#include "estruturas.h"
#include <pthread.h>

extern Editor editor;
extern int active_users;
extern Editor clients[3];
extern pthread_mutex_t lock;

int main(int argc, char *argv[]);
void getMAX_USERS(int n);
void updateAllUsersEditor();
bool find_username(char *username, char *filename);
void verify_env_var();
void SIGhandler(int sig);
void shutdown();
void *receiver();