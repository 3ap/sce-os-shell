#ifndef HELPER__H
#define HELPER__H

extern struct termios new_kbd_mode;
extern struct termios old_kbd_mode;

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define ARRAY_SIZE(array) \
    (sizeof(array) / sizeof(*array))

void set_unbuffered_io();
void set_buffered_io();

#endif
