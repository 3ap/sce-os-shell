#include <termios.h>
#include <string.h>

struct termios new_kbd_mode;
struct termios old_kbd_mode;

void set_unbuffered_io() {
  tcgetattr (0, &old_kbd_mode);
  memcpy (&new_kbd_mode, &old_kbd_mode, sizeof (struct termios));

  new_kbd_mode.c_lflag &= ~(ICANON | ECHO);
  new_kbd_mode.c_cc[VTIME] = 0;
  new_kbd_mode.c_cc[VMIN] = 1;
  tcsetattr (0, TCSANOW, &new_kbd_mode);
}

void set_buffered_io() {
  tcsetattr (0, TCSANOW, &old_kbd_mode);
}
