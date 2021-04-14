#include <stdio.h>
#include <string.h>

#include "helper.h"
#include "cmd.h"
#include "builtin.h"

#define PROMPT "> "

static char line[COMMAND_SIZE];

static command_t cmd1;
static command_t cmd2;

int main() {
  while(1) {
    fputs(PROMPT, stdout);
    if (fgets(line, ARRAY_SIZE(line), stdin) == NULL) {
	  fputs("\n", stdout);
	  break;
    }

    // if line includes only \n
    if (strlen(line) == 1)
      continue;

    cmd_parse(line, &cmd1);
    cmd_parse(NULL, &cmd2);

    if (cmd2.args[0] == NULL)
      // if there is only one command, run only first command
      cmd_run(&cmd1);
    else
      // otherwise, run two commands in a pipe
      cmd_pipe(&cmd1, &cmd2);
  }
  return 0;
}
