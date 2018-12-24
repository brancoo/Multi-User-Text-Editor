#include "estruturas.h"

extern Editor editor;
extern int active_users;
extern Editor clients[3];

int main(int argc, char *argv[]);
void getMAX_USERS(int n);
bool find_username(char *username, char *filename);
void verify_env_var();
void SIGhandler(int sig);
void shutdown();
void *receiver();
bool check_if_users_exceeds_max_active();
bool check_users_existence(char username[8]);
bool verify_line_edition(Editor aux);
void add_to_active_users_list(int pid, char username[8]);
void update_all_other_users(int pid);
void update_all_users();
