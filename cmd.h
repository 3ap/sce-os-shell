#ifndef CMD__H
#define CMD__H

#define MAX_ARGS     1024
#define COMMAND_SIZE 2048

typedef struct {
  char *args[MAX_ARGS];
  char *stdout_redirect;
} command_t;

int cmd_parse(char *line, command_t *cmd);
void cmd_run(command_t *cmd);
void cmd_print(command_t * const cmd);
void cmd_pipe(command_t *cmd1, command_t *cmd2);

#endif
