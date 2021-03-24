#ifndef BUILTIN__H
#define BUILTIN__H

#include "cmd.h"

typedef int builtin(int argc, char *argv[]);

builtin builtin_cd;
builtin builtin_pwd;
builtin builtin_exit;
builtin *is_builtin(command_t * const cmd);

#endif
