#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>

/* 
 * Takes command in shell as input, returns list of args separated by space
 */
int num_commands(char *cmd, char *c) {

		char *cmd_copy = malloc(strlen(cmd));
		strcpy(cmd_copy, cmd);

		// count number of words inputed (separated by spaces)
		int args = 0;
		char *token_copy = strtok(cmd_copy, c);
		while(token_copy) {
			args++;
			token_copy = strtok(NULL, c);
		}

		free(cmd_copy);
		return args;

}

/* 
 * Executes simple commands
 */

void execute_simple(char *argv[]) {

	// create child by running fork()
	pid_t pid;
	int status;

	pid = fork();

	if (pid < 0) { // error
		printf("ERROR: cannot fork\n");
	}

	else if (pid == 0) { // child process
		execvp(argv[0], argv);
		printf("ERROR NUMBER %d\n", errno);
		exit(errno);
	}

	else { // parent process
		while ((pid != wait(&status)));
	}	
}

int main(int argc, char *argv) {

	while (1) {

		char *cmd = readline("myshell> "); // read from command line
		int sem_args = num_commands(cmd, ";"); // number of items separated by ;
		int args; // number of args for cmd separated by space
		int i;
		int j; // loop variable

		char *cmd_copy = malloc(strlen(cmd)); //create copy of command string in shell
		strcpy(cmd_copy, cmd);

		char *sem_tks[sem_args]; // create array of commands separated by ;

		// split by semicolons 
		char *token_semi = strtok(cmd_copy, ";");

		i = 0;
		while(token_semi) {

			sem_tks[i] = token_semi; // add current tokenized string to array
			i++;
			token_semi = strtok(NULL, ";"); // move pointer over

		}

		for (j = 0; j < sem_args; j++) { // iterate through every command separated by ;

			char *tks[num_commands(sem_tks[j], " ") + 1]; // create array of tokens separate by space, with last space allotted to NULL

			// split by white space
			char *token_space = malloc(strlen(sem_tks[j])); // create copy of current command args
			strcpy(token_space, sem_tks[j]);
			char *token = strtok(token_space, " \n");

			i = 0;
			while(token) { 

				tks[i] = token; // add current arg string to array
				i++;
				token = strtok(NULL, " ");
			}
			tks[i] = (char *)0;

			// create child process and run simple commands
			execute_simple(tks);
		}
		

		return 0;
	}
}