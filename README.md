# Write Your Own Shell

## Authors

  - Alina Hlazkova  (346316979)
  - Sergey Nazaryev (342537685)

## Usage

```bash
make shell
./shell
```

## Introduction

The object of this assignment is to gain experience with some advanced
programming  techniques  such  as  process  creation,  termination,  and
overriding  an  existing  process  (`fork`, `wait`, `exec`, and additional system
calls). To do this, you will be writing your own command shell — much like bash.

## What is Shell?

A shell program is an application that allows interacting with the computer. In
a shell, the user can run programs and also redirect  the input  and  output.
Shell additionally offers features such as line editing, history, file
completion, etc.

"A shell is an interface that allows you to interact with the
kernel of an operating system."

## Shell Workflow

A shell parses commands entered by the user and executes them.
The workflow of the shell will look like this:

  1. Startup the shell
  2. Wait for user input
  3. Parse user input
  4. Execute the command and return the result
  5. Go back to 2

The shell is the parent process. This is the main process in our program which
is waiting for user input. However, we cannot execute the command in the parent
process itself, because of the following reasons:

  1. A  command  with  error will cause the shell to stop working.
     We want to avoid this.

  2. Independent commands  should  have  their  own  process blocks.
     This is known as the isolation principle.

## Required Features

Here are the features you should support in your shell:

  1)  The shell should indicate the current working directory using `pwd`.

  2)  `cd <path>` should change the working directory

  3)  Create a file by the command `cat > <filename>` or `nano <filename>`.

  4)  `cat <filename>`. View the contents of the file.

  5)  `wc [options/flags] filename`. The wc command should support the following flags
      `wc -l <filename>`,  `wc -c <filename>`,  `wc -w <filename>`.

  6)  `cp <file1> <file2>`. Copy the file <file1> to file <file2>

  7)  The shell has to support pipe operator "|" between processes.

  8)  `whoami`. Display the username of the current user when this command is
      invoked.

  9)  `grep`. Return lines with a specific word/string/pattern in a file.
      `grep [options/flags] [Pattern] <filename>`.
      Should support in the following options(flags): `grep -c [pattern] <filename>`

  10) `man [command  name]`. A  man  command  can  have  one  argument (command
      name) and then it will display a manual only for the commands that you have to
      implement in your own shell; man without arguments will display a manual for
      all the commands we requested to execute in this assignment.

  11) `ls` command lists directory contents of files and directories. `ls -l` show
      long  listing information about the file/directory.

  12) `tree` print all files. It is used to display the contents of any desired
      directory of your computer system in the form of a tree structure. By using
      this tree structure, you can easily find out the paths of your files and
      directories.
