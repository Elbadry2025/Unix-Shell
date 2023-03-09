# Unix-Shell
Implementing unix shell using C language
• Description:
This Code is an implementation of a Unix shell program,
that allows you to run other programs.
The code is composed of 7 functions including the main
function:
1. void setup_enviroment().
2. void parse_input().
3. execute_shell_bultin().
4. void execute_command()
5. void on_child_exit()
6. void shell()
7. int main().
• Major functions:
1. void setup_enviroment() :
It is used to prepare the shell at first start, by setting the
working directory to the default one which is the
Desktop.
2. void parse_input():
its used to prepare the input to be ready to be entered to
execvp() function, by splitting the input string into
multiple strings separated by “ “ where always args[0]
contains the command the rest contains the options of
the command, in this function we detect if there is ‘&’
which means that there will be a program which willrun on the background,this function also handle the
situation of using ‘$’ to call a variable from the
environment.
3. execute_shell_bultin():
In this function we execute commands which are shell
built-in commands as “cd”,”echo”,”export”, this
function handles these commands in C functions.
4. void execute_command():
In this function we execute the Unix commands as “ls”,
“touch”,”mkdir”.....etc, this function depends mainly
on the use of fork(), to have parent and child processes,
and use of execvp() for Unix commands, and the
waitpid() to choose to wait for the child process or no
depending if the process is in the background or not.
5. void on_child_exit():
This function is used to wait for child processes to exit
and reap their resources , especially the processes that
run on background, and log the termination to a log
file, and its called by signal SIGCHILD.
6. void shell():
It contains most of the previous functions, to run the
shell, it contains a do while loop which is stopped only
by exit command, in this loop it checks whether the
command is built-in or Unix command to execute the
proper function.7. int main():
The signal SIGCHILD is registered in it, also it uses
the setup_enviroment() to prepare the shell, then it
executes the shell itself.
