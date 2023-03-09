#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define MAX_ARGS 10 // maximum number of command line arguments
#define MAX_LENGTH 100 // maximum length of input line


char input[MAX_LENGTH]; // buffer to hold input line
char inputCOpy[MAX_LENGTH];
char *args[MAX_ARGS]; // array to hold command line arguments
int k;
int initial = 0;
int flag =0;

// This function is named on_child_exit and has a return type of void.
void on_child_exit(){
    int status;
    pid_t pid;
    // This is an infinite loop that waits for child processes to exit.
    while (1)
    {
        // The wait3 system call is used to wait for child processes to exit.
        // The status of the child process is stored in the status variable.
        // The WNOHANG flag is passed as an argument to wait3 to prevent blocking.
        pid = wait3(&status, WNOHANG, (struct rusage *)NULL);
        // If the process ID of the child is 0 or -1, exit the loop.
        if (pid == 0 || pid == -1)
            break;  // Exit the loop.
    }

    // Open a file called output.txt and append to it.
    FILE *fp;
    fp = fopen("/home/elbadry/Desktop/output.txt", "a");
    // Write "Child Terminated" to the file.
    fprintf(fp, "Child Terminated\n");
    // Close the file.
    fclose(fp);
}

void setup_enviroment(){
    char cwd[1024];

    // Change the current working directory to /home/elbadry/Desktop
    if (chdir("/home/elbadry/Desktop") != 0) {
        perror("chdir() error"); // Print an error message if chdir() failed
        return ; // Exit the function if chdir() failed
    }

    // Get the new current working directory
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error"); // Print an error message if getcwd() failed
        return ; // Exit the function if getcwd() failed
    }

    // Print the new current working directory in green color
    printf("\033[1;32m");
    printf("%s", cwd);
    printf("\033[0m"); // Reset color to default
}

void parse_input(){
    // Check if this is the first input, and print the current working directory
    if(initial == 1){
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) == NULL) { // getcwd() gets the current working directory and stores it in cwd array
            perror("getcwd() error"); // if getcwd() fails, print error message
            return ;
        }
        printf("\033[1;32m"); // set text color to green
        printf("%s", cwd); // print the current working directory
        printf("\033[0m"); // reset text color
    }
    initial = 1; // set initial flag to 1 to indicate that the first input has been processed

    printf("\033[1;31m"); // set text color to red
    printf(" > "); // print the prompt character
    printf("\033[0m"); // reset text color

    fgets(input, MAX_LENGTH, stdin); // read a line of input from the user and store it in the input array
    input[strcspn(input, "\n")] = 0; // remove the trailing newline character from the input

    strcpy(inputCOpy,input); // copy the input string to another array for further processing

    // Check if the last character in the input is an ampersand, which indicates that the command should run in the background
    if(inputCOpy[strlen(inputCOpy)-1] == '&'){
        flag = 1;
    }else{
        flag = 0;
    }

    char *token = strtok(input, " "); // get the first token in the input string

    // Loop through the tokens in the input string and store them in the args array
    for(k = 0; k < MAX_ARGS - 1 && token != NULL && strcmp(token,"&"); k++) {
        args[k] = token; // store token in args array
        token = strtok(NULL, " "); // get the next token
    }

    args[k] = NULL; // set the last element of the args array to NULL, as required by execvp()

    if(k>=2){
    char *dollar[] = {"$",NULL};
    char myCHar = *(args[1]);
    for(int h=0 ; h<k ; h++){
        if(*(args[h]) == '$'){ // if argument starts with $
            char *x[] = {*(args[h]+1),NULL}; // get the argument without the $
            int q;
            char *y = getenv(x) ; // get the value of the environment variable
            char *token = strtok(y, " "); // get first token
            for(q = h; q < MAX_ARGS - 1 && token != NULL; q++) {
                args[q] = token; // replace the $variable with its value
                token = strtok(NULL, " "); // get next token
            }
            args[q] = NULL; // set last element to NULL for execvp
        }
    }
}


}
void execute_shell_bultin(){
    // Check if the command is a shell builtin
    if(strcmp(args[0],"cd")==0){
        // If it's a cd command, change the current working directory
        char cwd[1024];
        if (chdir(args[1]) != 0) {
            // If chdir returns an error, print the error message
            perror("chdir() error");
            return ;
        }
        // Get the current working directory and print it
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            // If getcwd returns an error, print the error message
            perror("getcwd() error");
            return ;
        }
    }else if(strcmp(args[0],"echo")==0){
        // If it's an echo command, print the arguments
        for(int h=1 ;args[h]!=NULL;h++){
            printf("%s ",args[h]);
        }
        printf("\n");
    }else if(strcmp(args[0],"export")==0){
        // If it's an export command, set an environment variable
        // Parse the argument to get the variable name and value
        char *x = strtok(args[1],"=");
        strtok(inputCOpy,"=");
        char *y = strtok(NULL,"=");
        // Remove double quotes from the value string
        int len = strlen(y);
        for (int i = 0; i < len; i++) {
            if (y[i] == '\"') { // if current character is double quote
                for (int j = i; j < len; j++) {
                    y[j] = y[j+1]; // shift all characters one position left
                }
                len--; // decrease the length of the string
                i--; // decrement the index i to check the current position again
            }
        }
        // Set the environment variable using setenv
        setenv(x,y,1);
    }
}

void execute_command(){
    // Fork a new process
    pid_t child_id = fork();
    int status;
    if(child_id == 0){
        // Child process
        // Execute the command using execvp
        execvp(args[0],args);
        // If execvp returns, an error occurred
        printf("Error\n");
        exit(1);
    }else if(child_id > 0 ){
        // Parent process
        // Wait for the child process to complete
        if( flag == 1){
            // If the flag is set, use WNOHANG option to not block and wait for child process
            waitpid(child_id, &status, WNOHANG);
        } else{
            // Otherwise, block and wait for child process to complete
            waitpid(child_id, &status,0);
        }
    }else{
        // Error occurred when forking
        printf("Error\n");
        exit(1);
    }
}

void shell(){
    // Keep looping until user enters "exit"
    do{
        // Parse user input
        parse_input();

        // Check if the command is a shell builtin
        if(strcmp(args[0],"export")==0 || strcmp(args[0],"echo")==0 || strcmp(args[0],"cd")==0){
            // If so, execute the builtin command
            execute_shell_bultin();
        }
        // If the command is not a shell builtin and not "exit"
        else if(strcmp(args[0],"exit")!=0){
            // Execute the command
            execute_command();
        }

    }while(strcmp(args[0],"exit")!=0); // Keep looping until user enters "exit"
}

int main()
{
    // Register signal handler for SIGCHLD
    signal(SIGCHLD,on_child_exit);

    // Set up environment (i.e., change current working directory to the default location)
    setup_enviroment();

    // Start the shell program
    shell();

    // Return 0 to indicate successful execution of the program
    return 0;
}
