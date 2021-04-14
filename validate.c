/*
 * Alina Hlazkova  (346316979)
 * Sergey Nazaryev (342537685)
*/

#include <stdio.h>
#include <string.h>

#include "cmd.h"
#include "validate.h"
#include "helper.h"

static int validate_cd(command_t *cmd);
static int validate_cat(command_t *cmd);
static int validate_nano(command_t *cmd);
static int validate_wc(command_t *cmd);
static int validate_cp(command_t *cmd);
static int validate_grep(command_t *cmd);
static int validate_man(command_t *cmd);
static int validate_ls(command_t *cmd);

static int validate_zero_args(command_t *cmd);
static int validate_noop(command_t *cmd);

static struct validate_info cmds[] = {
  { "pwd",    validate_zero_args },
  { "cd",     validate_cd        },
  { "cat",    validate_cat       },
  { "nano",   validate_nano      },
  { "wc",     validate_wc        },
  { "cp",     validate_cp        },
  { "whoami", validate_zero_args },
  { "grep",   validate_grep      },
  { "man",    validate_man       },
  { "ls",     validate_ls        },
  { "tree",   validate_zero_args },
  { "exit",   validate_zero_args },
};

static int validate_flags(command_t *cmd, char *valid_flags, int maximum_positional_args, int minimal_positional_args) {
    int flag_end = 0;

    int argc = cmd_argslen(cmd);
    int positional_args = 0;
    int valid_flags_count = strlen(valid_flags);
    int i, j;

    for (i = 1; i < argc; i++)
    {
        if (cmd->args[i][0] == '-' && !flag_end) {
            char flag = cmd->args[i][1];
            int flag_found = 0;

            for (j = 0; j < strlen(valid_flags); j++)
            {
                if (flag == valid_flags[j])
                {
                    flag_found = 1;
                    break;
                }
            }

            if (!flag_found)
            {
                fprintf(stderr, "error: there is no flag -%c for `%s`\n", flag, cmd->args[0]);
                return 0;
            }
        } else {
            positional_args++;
            flag_end = 1;
        }
    }

    if (positional_args > maximum_positional_args ||
        positional_args < minimal_positional_args)
        return 0;

    return 1;
}

static int validate_noop(command_t *cmd)
{
    return 1;
}

static int validate_zero_args(command_t *cmd)
{
    if (cmd_argslen(cmd) != 1) {
        fprintf(stderr, "usage: %s\n", cmd->args[0]);
        return 0;
    }

    return 1;
}

static int validate_nano(command_t *cmd)
{
    if (!validate_flags(cmd, "", 1, 1))
    {
        fprintf(stderr, "usage: %s <PATH>\n", cmd->args[0]);
        return 0;
    }

    return 1;
}

static int validate_cat(command_t *cmd)
{
    if (!validate_flags(cmd, "", 1, 0))
    {
        fprintf(stderr, "usage: %s <PATH>\n", cmd->args[0]);
        return 0;
    }

    return 1;
}

static int validate_wc(command_t *cmd)
{
    if (!validate_flags(cmd, "lcw", 1, 1))
    {
        fprintf(stderr, "usage: %s [OPTION] <FILE>\n", cmd->args[0]);
        fprintf(stderr, "\n");
        fprintf(stderr, "-c		print the byte counts\n");
        fprintf(stderr, "-l		print the newline counts\n");
        fprintf(stderr, "-w		print the word counts\n");
        return 0;
    }

    return 1;
}

static int validate_cp(command_t *cmd)
{
    if (!validate_flags(cmd, "", 2, 2))
    {
        fprintf(stderr, "usage: %s <FILE1> <FILE2>\n", cmd->args[0]);
        return 0;
    }

    return 1;
}

static int validate_grep(command_t *cmd)
{
    if (!validate_flags(cmd, "c", 2, 2))
    {
        fprintf(stderr, "usage: %s [OPTION] <PATTERN> <FILE>\n", cmd->args[0]);
        fprintf(stderr, "\n");
        fprintf(stderr, "-c 	Only print the number of matches in file.\n");
        return 0;
    }

    return 1;
}

static int validate_ls(command_t *cmd)
{
    if (!validate_flags(cmd, "l", 0, 0))
    {
        fprintf(stderr, "usage: %s [OPTION]\n", cmd->args[0]);
        fprintf(stderr, "\n");
        fprintf(stderr, "-l 	use a long listing format\n");
        return 0;
    }

    return 1;
}

static int validate_cd(command_t *cmd)
{
    if (!validate_flags(cmd, "", 1, 1))
    {
        fprintf(stderr, "usage: %s <PATH>\n", cmd->args[0]);
        return 0;
    }

    return 1;
}

static int validate_man(command_t *cmd)
{
    if (cmd_argslen(cmd) != 2)
    {
        fprintf(stderr, "usage: %s <COMMAND>\n", cmd->args[0]);
        return 0;
    }

    if (validate_info_get(cmd->args[1]) == NULL)
    {
        fprintf(stderr, "error: there is no man for `%s`\n", cmd->args[1]);
        return 0;
    }

    return 1;
}

struct validate_info *validate_info_get(char * const cmdname)
{
    int i;
    for (i = 0; i < ARRAY_SIZE(cmds); i++)
        if (!strcmp(cmds[i].cmdname, cmdname))
            return &cmds[i];

    return NULL;
}
