#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include "helper.h"
#include "builtin.h"
#include "cmd.h"

static int cmd_argslen(command_t * const cmd) {
  int len = 0;
  for(len = 0; cmd->args[len] != NULL; len++);
  return len;
}

int cmd_parse(char *line, command_t *cmd)
{
  // fprintf(stderr, "cmdline: %s", line);
  cmd->stdout_redirect = NULL;

  bool prev_is_space = false;

  size_t len = strlen(line);
  int argcnt = 0;

  char *curargptr = line;
  for(int i = 0; i < len; i++) {
    if (line[i] == ' ' && !prev_is_space) {
      line[i] = '\0';
      cmd->args[argcnt++] = curargptr;
      prev_is_space = true;
    } else if (line[i] == ' ' && prev_is_space) {
      continue;
    } else if (line[i] != ' ' && prev_is_space) {
      prev_is_space = false;
      curargptr = line + i;
    } else if (line[i] == '\n') {
      line[i] = '\0';
      cmd->args[argcnt++] = curargptr;
    }
  }

  cmd->args[argcnt++] = NULL;
  for(int i = 0; cmd->args[i] != NULL; i++) {
    if (cmd->args[i][0] == '>' && cmd->args[i+1] != NULL) {
      cmd->stdout_redirect = cmd->args[i+1];
      cmd->args[i] = NULL;
    }
  }
}

void cmd_run(command_t *cmd)
{
  pid_t pid;
  int status;
  builtin *cmdptr;

  if ((cmdptr = is_builtin(cmd)) != NULL) {
    dup2(1, 10);
    int fd = open(cmd->stdout_redirect, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    dup2(fd, 1);
    close(fd);
    cmdptr(cmd_argslen(cmd), cmd->args);
    dup2(10, 1);
    close(10);
  } else {
    pid = fork();

    if (pid == -1) {
      perror("Failed to fork proccess");
    }

    if (pid > 0) {
      waitpid(pid, &status, 0);
    } else if (pid == 0) {
      if (cmd->stdout_redirect) {
        int fd = open(cmd->stdout_redirect, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	dup2(fd, 1);
	close(fd);
      }

      if (execvp(cmd->args[0], cmd->args) == -1) {
        perror("Failed to exec");
        _exit(2);
      }
    }
  }
}

void cmd_print(command_t * const cmd)
{
  printf("cmd: %s\n", cmd->args[0]);
  for(int i = 0; cmd->args[i] != NULL; i++)
    printf("args[%d] = %s\n", i, cmd->args[i]);
}
