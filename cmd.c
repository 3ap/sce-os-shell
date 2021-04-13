#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include <unistd.h>

#include "helper.h"
#include "builtin.h"
#include "cmd.h"
#include "validate.h"

int cmd_argslen(command_t * const cmd)
{
  int len = 0;
  for(len = 0; cmd->args[len] != NULL; len++);
  return len;
}

int cmd_parse(char *cmdline, command_t *cmd)
{
  int prev_is_space = 1;
  static char *line = NULL;
  int pos, i, argcnt;
  size_t len;
  char *curargptr;

  if (cmdline != NULL)
    line = cmdline;

  len = strlen(line);
  argcnt = 0;

  for(pos = 0; pos < len; pos++) {
    if (line[pos] != ' ' && prev_is_space) {
      curargptr = line + pos;
      prev_is_space = 0;
      if (line[pos] == '|') {
        line = line + pos + 1;
        break;
      }
    } else if (line[pos] == ' ' && !prev_is_space) {
      line[pos] = '\0';
      cmd->args[argcnt++] = curargptr;
      prev_is_space = 1;
    } else if (line[pos] == ' ' && prev_is_space) {
      continue;
    } else if (line[pos] == '\n') {
      line[pos] = '\0';
      cmd->args[argcnt++] = curargptr;
    } else if (line[pos] == '|') {
      line[pos] = '\0';
      cmd->args[argcnt++] = curargptr;
      line = line + pos + 1;
      break;
    }
  }

  cmd->stdout_redirect = NULL;
  cmd->args[argcnt++]  = NULL;
  for(i = 0; cmd->args[i] != NULL; i++) {
    if (cmd->args[i][0] == '>' && cmd->args[i+1] != NULL) {
      cmd->stdout_redirect = cmd->args[i+1];
      cmd->args[i] = NULL;
    }
  }

  return 0;
}

void cmd_pipe(command_t * const cmd1, command_t * const cmd2) {
  int pipefd[2];
  pid_t pid1, pid2;
  int status1, status2;

  if (pipe(pipefd) == -1) {
    perror("Failed to create pipe");
    return;
  }

  pid1 = fork();
  if (pid1 == -1) {
    perror("Failed to fork proccess");
    close(pipefd[0]);
    close(pipefd[1]);
    return;
  }

  if (pid1 > 0) {
    pid2 = fork();
    if (pid2 == -1) {
      perror("Failed to fork proccess");
      close(pipefd[0]);
      close(pipefd[1]);
      // TODO: kill child 1
    }

    if (pid2 > 0) {
      // parent: close fd[0], fd[1], wait for childrens
      signal(SIGINT, SIG_IGN);
      close(pipefd[0]);
      close(pipefd[1]);
      waitpid(pid1, &status1, 0);
      waitpid(pid2, &status2, 0);
      signal(SIGINT, SIG_DFL);
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

int cmd_validate(command_t * const cmd)
{
    struct validate_info *info = validate_info_get(cmd->args[0]);
    if (info == NULL) {
        fprintf(stderr, "Command `%s` is not supported\n", cmd->args[0]);
        return 0;
    }

    if (!info->validate(cmd))
        return 0;

    return 1;
}

void cmd_run(command_t * const cmd)
{
  pid_t pid;
  int status, fd;
  builtin *cmdptr;

  if ((cmdptr = is_builtin(cmd)) != NULL) {
    if (!cmd_validate(cmd))
      return;

    dup2(1, 10);
    fd = open(cmd->stdout_redirect, O_WRONLY|O_CREAT|O_TRUNC, 0666);
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
      signal(SIGINT, SIG_IGN);
      waitpid(pid, &status, 0);
      signal(SIGINT, SIG_DFL);
    } else if (pid == 0) {
      if (!cmd_validate(cmd))
        _exit(2);

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
