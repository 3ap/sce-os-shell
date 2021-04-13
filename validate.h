#ifndef VALIDATE__H
#define VALIDATE__H

#include "cmd.h"

typedef int validate_proc(command_t * const cmd);

struct validate_info {
  char *cmdname;
  validate_proc *validate;
};

struct validate_info *validate_info_get(char * const cmd);

#endif
