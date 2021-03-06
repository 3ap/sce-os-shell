/*
 * Alina Hlazkova  (346316979)
 * Sergey Nazaryev (342537685)
*/

#ifndef CMD__H
#define CMD__H

#define MAX_ARGS     1024
#define COMMAND_SIZE 2048

typedef struct {
  char *args[MAX_ARGS];
  char *stdout_redirect;
} command_t;

int cmd_parse(char *line, command_t *cmd);
void cmd_run(command_t * const cmd);
void cmd_pipe(command_t * const cmd1, command_t * const cmd2);
int cmd_validate(command_t * const cmd);
int cmd_argslen(command_t * const cmd);

#endif
