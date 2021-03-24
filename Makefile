TARGET = shell

$(TARGET): shell.c builtin.c cmd.c helper.c

clean:
	rm -f $(TARGET)

.PHONY: clean
