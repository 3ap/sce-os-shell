TARGET = shell

$(TARGET): shell.c builtin.c cmd.c

clean:
	rm -f $(TARGET)

.PHONY: clean
