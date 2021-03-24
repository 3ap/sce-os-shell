#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "helper.h"
#include "cmd.h"
#include "builtin.h"

#define PROMPT "> "

#define HISTORY_SIZE 1024

#define KEYSTROKE_UP        0
#define KEYSTROKE_DOWN      1
#define KEYSTROKE_BACKSPACE 2
#define KEYSTROKE_EOF       3
#define KEYSTROKE_CTRLU     4
#define KEYSTROKE_HOME      5
#define KEYSTROKE_END       6

static char erase_line[COMMAND_SIZE];

static char line[COMMAND_SIZE];
static int  linecnt;

static char history[HISTORY_SIZE][COMMAND_SIZE];
static int  histcnt = 0;
static int  histcur = 0;

static command_t cmd;

static int is_keystroke(char * const line, int size) {
  if (size == 1) {
    if (line[0] == 0x7F)
      return KEYSTROKE_BACKSPACE;
    else if (line[0] == 0x4)
      return KEYSTROKE_EOF;
    else if (line[0] == 0x15)
      return KEYSTROKE_CTRLU;
  }

  if (size == 3) {
    if (line[0] == 0x1B && line[1] == 0x5B && line[2] == 0x41)
      return KEYSTROKE_UP;
    else if (line[0] == 0x1B && line[1] == 0x5B && line[2] == 0x42)
      return KEYSTROKE_DOWN;
    else if (line[0] == 0x1B && line[1] == 0x5B && line[2] == 0x48)
      return KEYSTROKE_HOME;
    else if (line[0] == 0x1B && line[1] == 0x5B && line[2] == 0x46)
      return KEYSTROKE_END;
  }

  return -1;
}

static int process_keystroke() {
  int ret = -1;

  if (linecnt >= 1) {
    if ((ret = is_keystroke(line + linecnt - 1, 1)) != -1) {
      linecnt = linecnt - 1;
      return ret;
    }
  }

  if (linecnt >= 3) {
    if ((ret = is_keystroke(line + linecnt - 3, 3)) != -1) {
      linecnt = linecnt - 3;
      return ret;
    }
  }

  return ret;
}

static void clear_line(int linesz) {
  fputs("\r", stdout);
  fwrite(erase_line, sizeof(char), linesz, stdout);
  fputs("\r", stdout);
}

static void history_up() {
  if (histcur > 0) {
    histcur--;

    int max = MAX(strlen(line), strlen(history[histcur]));

    strncpy(line, history[histcur], ARRAY_SIZE(line));
    linecnt = strlen(line) - 1;

    clear_line(max + strlen(PROMPT));
    fputs(PROMPT, stdout);
    fwrite(line, sizeof(char), linecnt, stdout);
  }
}

static void history_down() {
  if (histcur < histcnt - 1) {
    histcur++;

    int max = MAX(strlen(line), strlen(history[histcur]));

    strncpy(line, history[histcur], ARRAY_SIZE(line));
    linecnt = strlen(line) - 1;

    clear_line(max + strlen(PROMPT));
    fputs(PROMPT, stdout);
    fwrite(line, sizeof(char), linecnt, stdout);
  }
}

static char *read_line(char *line) {
  linecnt = 0;

  while(1) {
    int cur = linecnt;
    int c = getc(stdin);

    line[cur] = c;
    linecnt++;

    if (c == 0x1B) {
      line[cur + 1] = getc(stdin);
      line[cur + 2] = getc(stdin);
      linecnt += 2;
    }

    int keystroke = -1;
    if ((keystroke = process_keystroke()) != -1) {
      if (keystroke == KEYSTROKE_BACKSPACE) {
        if (linecnt > 0) {
          fputs("\b \b", stdout);
          linecnt--;
        }
        continue;
      } else if (keystroke == KEYSTROKE_EOF) {
        fputs("\n", stdout);
        builtin_exit(0, NULL);
        break;
      } else if (keystroke == KEYSTROKE_UP) {
	history_up();
        continue;
      } else if (keystroke == KEYSTROKE_DOWN) {
	history_down();
        continue;
      } else if (keystroke == KEYSTROKE_CTRLU) {
        clear_line(linecnt + strlen(PROMPT));
        fputs(PROMPT, stdout);
        linecnt = 0;
        continue;
      } else if (keystroke == KEYSTROKE_HOME) {
      } else if (keystroke == KEYSTROKE_END) {
      }
    }

    putc(c, stdout);
    if (line[cur] == '\n') {
      line[cur+1] = '\0';
      break;
    }
  }

  return line;
}

int main() {
  for (int i = 0; i < ARRAY_SIZE(erase_line); i++)
    erase_line[i] = ' ';

  set_unbuffered_io();

  while(1) {
    histcur = histcnt;

    fputs(PROMPT, stdout);
    read_line(line);
    if (strlen(line) == 1)
      continue;

    strncpy(history[histcnt++], line, ARRAY_SIZE(history[0]));
    cmd_parse(line, &cmd);
    cmd_run(&cmd);
  }

  set_buffered_io();
  return 0;
}
