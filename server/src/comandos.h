#include <stdbool.h>

void cmd(char *com);
void settings();
bool verify_file_existence(char *file);
void save_settings(char *filename);
void free_row(int line);
void load_file(char *filename);
void statistics();
void users();
void text();
void shutdown();
void initialize_editor_content();