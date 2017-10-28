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

void execute_simple(char *argv[], char *file) {

	// create child by running fork()
	pid_t pid;
	int status;
	FILE *fp;

	pid = fork();

	if (pid < 0) { // error
		printf("ERROR: cannot fork\n");
	}

	else if (pid == 0) { // child process
		if (file != NULL)
		{
			fp = fopen(file, "w");
			dup2(fileno(fp), STDOUT_FILENO);
		}
		execvp(argv[0], argv);
		fclose(fp);
		printf("ERROR NUMBER %d\n", errno);
		exit(errno);
	}

	else { // parent process
		while ((pid != wait(&status)));
	}	
}

int main(int argc, char *argv) {

	char *FILE = NULL;
	int carat_out = 0;

	while (1) {
		char *cmd = readline("myshell> "); // read from command line
		

		int sem_args = num_commands(cmd, ";"); // number of items separated by ;
		int args; // number of args for cmd separated by space
		int i;
		int j; // loop variable

		char *cmd_copy = malloc(strlen(cmd)); //create copy of command string in shell
		strcpy(cmd_copy, cmd);

		char *sem_tks[sem_args]; // create array of commands separated by ;

		// //split by semicolons 
		// halves = strtok(cmd_copy, ">");
		// char *commd = malloc(strlen(cmd));
		// strcpy(commd, halves);
		// printf("command: %s\n", commd);

		// halves = strtok(NULL, ">");
		// FILE = malloc(strlen(cmd_copy));
		// strcpy(FILE, halves);
		// FILE = strtok(FILE, " ");
		// printf("file:%s\n", FILE);

		// char *token_semi;

		// if (FILE != NULL){
		// 	token_semi = commd;	
		// }

		char *token_semi = strtok(cmd_copy, ";");

		i = 0;
		while(token_semi) {

			sem_tks[i] = token_semi; // add current tokenized string to array
			i++;
			token_semi = strtok(NULL, ";"); // move pointer over

		}


		for (j = 0; j < sem_args; j++) { // iterate through every command separated by ;

			char *token_space = malloc(strlen(sem_tks[j])); // create copy of current command args
			strcpy(token_space, sem_tks[j]);
			char *commd;

			if (strchr(sem_tks[j], '>')){
				carat_out = 1;
			}

			printf("%d\n", carat_out);
			if (carat_out) {
				char *carat_cmd;

				//split by semicolons 
				carat_cmd = strtok(token_space, ">"); // get first command before >
				commd = malloc(strlen(carat_cmd));
				strcpy(commd, carat_cmd);
				printf("command: %s\n", commd);

				carat_cmd = strtok(NULL, ">"); // get file
				FILE = malloc(strlen(carat_cmd));
				strcpy(FILE, carat_cmd);
				FILE = strtok(FILE, " ");
				printf("file:%s\n", FILE);

			}

			int size;
			char *token;
			if (carat_out) {
				size = num_commands(commd, " ") + 1;// create array of tokens separate by space, with last space allotted to NULL
				token = strtok(commd, " \n");
			}

			else {
				size = num_commands(sem_tks[j], " ") + 1;
				token = strtok(sem_tks[j], " \n");
			}

			char *tks[size];

			i = 0;
			while(token) { 
				printf("token: %s\n", token);
				tks[i] = token; // add current arg string to array
				i++;
				token = strtok(NULL, " ");
			}
			tks[i] = (char *)0;

			// create child process and run simple commands
			execute_simple(tks, FILE);
		}
		

		return 0;
	}
}