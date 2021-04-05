#ifndef VALIDATE__H
#define VALIDATE__H

#include <stdbool.h>
#include "cmd.h"

typedef bool validate_proc(command_t * const cmd);

struct validate_info {
  char *cmdname;
  validate_proc *validate;
};

struct validate_info *validate_info_get(char * const cmd);

#endif
