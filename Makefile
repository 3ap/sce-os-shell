.SUFFIXES:

TARGET  = shell
SOURCES = shell.c builtin.c cmd.c validate.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<
