TARGET = shell

$(TARGET): shell.c builtin.c cmd.c validate.c

clean:
	rm -f $(TARGET)

.PHONY: clean
