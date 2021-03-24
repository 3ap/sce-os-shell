#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#include "helper.h"
#include "builtin.h"
#include "cmd.h"

builtin *is_builtin(command_t * const cmd) {
  if (!strcmp(cmd->args[0], "cd"))
    return &builtin_cd;
  if (!strcmp(cmd->args[0], "pwd"))
    return &builtin_pwd;
  if (!strcmp(cmd->args[0], "exit"))
    return &builtin_exit;

  return NULL;
}

int builtin_cd(int argc, char *argv[]) {
  char *home = getenv("HOME");
  char *path = argc == 1 ? home : argv[1];
  if (chdir(path) == -1) {
    perror("Failed to change directory");
    return 1;
  }
  return 0;
}

int builtin_pwd(int argc, char *argv[]) {
  static char path[PATH_MAX];
  if (getcwd(path, ARRAY_SIZE(path)) == NULL) {
    perror("Failed to obtain current working directory");
    return 1;
  }

  fprintf(stdout, "%s\n", path);
  return 0;
}

int builtin_exit(int argc, char *argv[]) {
  set_buffered_io();
  _exit(0);
  return 0;
}
