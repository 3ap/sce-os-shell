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

int cmd_parse(char *cmdline, command_t *cmd)
{
  static char *line = NULL;
  int pos = 0;

  if (cmdline != NULL)
    line = cmdline;

  cmd->stdout_redirect = NULL;

  bool prev_is_space = false;

  size_t len = strlen(line);
  int argcnt = 0;

  char *curargptr = line;
  //fprintf(stderr, "cmdline: %s", curargptr);
  //fprintf(stderr, "len: %d\n", len);
  for(pos; pos < len; pos++) {
    if (line[pos] == ' ' && pos == 0) {
      curargptr++;
      continue;
    } else if (line[pos] == ' ' && !prev_is_space) {
      line[pos] = '\0';
      cmd->args[argcnt++] = curargptr;
      prev_is_space = true;
    } else if (line[pos] == ' ' && prev_is_space) {
      continue;
    } else if (line[pos] != ' ' && prev_is_space) {
      prev_is_space = false;
      curargptr = line + pos;
      if (line[pos] == '|') {
        line = line + pos + 1;
        break;
      }
    } else if (line[pos] == '\n') {
      // puts("change \\n");
      line[pos] = '\0';
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

void cmd_pipe(command_t *cmd1, command_t *cmd2) {
  int pipefd[2];
  pid_t pid1, pid2;
  int status1, status2;

  if (pipe(pipefd) == -1) {
    perror("Failed to create pipe");
    return;
  }

  pid1 = fork();
  if (pid1 > 0) {
    pid2 = fork();
    if (pid2 == -1) {
      perror("Failed to fork proccess");
      // TODO: kill child 1
    }

    if (pid2 > 0) {
      // parent: close fd[0], fd[1], wait for childrens
      close(pipefd[0]);
      close(pipefd[1]);
      waitpid(pid1, &status1, 0);
      waitpid(pid2, &status2, 0);
    } else {
      // child 2: close fd[0], stdout -> fd[1]
      close(pipefd[0]);
      dup2(pipefd[1], 1);
      close(pipefd[1]);
      cmd_run(cmd1);
      _exit(0);
    }
  } else {
    // child 1: close fd[1], stdin <- fd[0]
    close(pipefd[1]);
    dup2(pipefd[0], 0);
    close(pipefd[0]);
    cmd_run(cmd2);
    _exit(0);
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
      set_unbuffered_io();
    } else if (pid == 0) {
      set_buffered_io();

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
