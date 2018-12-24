#include "main.h"

bool find_username(char *username, char *filename);
bool check_if_users_exceeds_max_active();
bool check_users_existence(char username[8]);
bool verify_line_edition(Editor aux);
void add_to_active_users_list(int pid, char username[8]);
void update_all_other_users(int pid);
void update_all_users();
void delete_user_from_array(int pid);