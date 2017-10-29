#include <stdlib.h>
#include <stdio.h>
#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>

pid_t pid;

void sig_handler(int signo)
{
  if (signo == SIGINT)
    kill(pid, SIGINT);
}

/* 
 * Takes command in shell as input, returns list of args separated by space
 */
int num_commands(char *cmd, char *c) {
		if (cmd == NULL)
			exit(0);
		// printf("%s\n", cmd);
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
void execute_simple(char *argv[], char *file, int crt_in, int crt_out, char car_arg, int amp) {

	// printf("%d\n", amp);
	// create child by running fork()
	int status;
	FILE *fp;

	pid = fork();

	if (pid < 0) { // error
		printf("ERROR: cannot fork\n");
	}

	else if (pid == 0) { // child process
		if (file != NULL)
		{
			
			if (crt_out == 1 || (crt_out == 1 && car_arg == '1')){
				fp = fopen(file, "w");
				dup2(fileno(fp), STDOUT_FILENO);
			}

			if (crt_out == 1 && car_arg == '2'){
				fp = fopen(file, "w");
				dup2(fileno(fp), STDERR_FILENO);
			}

			if (crt_out == 1 && car_arg == '&'){
				fp = fopen(file, "w");
				dup2(fileno(fp), STDOUT_FILENO);
				dup2(fileno(fp), STDERR_FILENO);
			}

			if (crt_in == 1){
				fp = fopen(file, "r");
				dup2(fileno(fp), STDIN_FILENO);
			}

		}
		execvp(argv[0], argv);
		fclose(fp);
		printf("ERROR NUMBER %d\n", errno);
		exit(errno);
	}

	else { // parent process
		if (amp == 0){
			waitpid(pid, &status, 0);
			//while ((pid != wait(&status)));	
		}
	}	
}

void main(int argc, char *argv) {

	struct sigaction sigact;
	sigact.sa_handler = sig_handler;
	sigaction(SIGINT, &sigact, NULL);

	while (1) {
		int amp = 0;
		char *FILE = NULL;
		int carat_out = 0;
		int carat_in = 0;
		char car_arg = '0';

		char *cmd = readline("myshell> "); // read from command line

		// printf("h\n");
		int sem_args = num_commands(cmd, ";"); // number of items separated by ;
		int args; // number of args for cmd separated by space
		int i;
		int j; // loop variable

		char *cmd_copy = malloc(strlen(cmd)); //create copy of command string in shell
		strcpy(cmd_copy, cmd);

		char *sem_tks[sem_args]; // create array of commands separated by ;

		char *token_semi = strtok(cmd_copy, ";");

		i = 0;
		while(token_semi) {

			sem_tks[i] = token_semi; // add current tokenized string to array
			i++;
			token_semi = strtok(NULL, ";"); // move pointer over

		}


		for (j = 0; j < sem_args; j++) { // iterate through every command separated by ;

			char *token_space = malloc(strlen(sem_tks[j])+1); // create copy of current command args
			strcpy(token_space, sem_tks[j]);
			token_space[strlen(token_space)] = '\0';
			char *commd;
			char *c = malloc(3);

			if (strchr(sem_tks[j], '>')){
				carat_out = 1;
				strcpy(c, ">");
			}

			if (strchr(sem_tks[j], '<')){
				carat_in = 1;
				strcpy(c,"<");
			}

			if ((strchr(sem_tks[j], '&')  != NULL) && (strchr(sem_tks[j], '>') == NULL)){
				printf("Recognized &\n");
				amp = 1;
				strcpy(c,"&");
				sem_tks[j] = strtok(sem_tks[j], "&");
			}
			printf("%s\n", sem_tks[j]);


			//printf("%d\n", carat_out);
			if (carat_out || carat_in) {
				char *carat_cmd;
				//split by semicolons 
				carat_cmd = strtok(token_space, c); // get first command before >
				commd = malloc(strlen(carat_cmd));
				strcpy(commd, carat_cmd);
				if (strstr(commd, " 1")){
					car_arg = '1';
				}
				if (strstr(commd, " 2")){
					car_arg = '2';
				}
				if (strstr(commd, " &")){
					car_arg = '&';
				}

				carat_cmd = strtok(NULL, c); // get file
				FILE = malloc(strlen(carat_cmd)+1);
				strcpy(FILE, carat_cmd);
				FILE = strtok(FILE, " ");
				FILE[strlen(FILE)] = '\0';

				if (car_arg != '0'){
					commd[strlen(commd)-1] = 0;
				}

			}

			int size;
			char *token;
			if (carat_out || carat_in) {
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
				//printf("token: %s\n", token);
				tks[i] = token; // add current arg string to array
				i++;
				token = strtok(NULL, " ");
			}
			tks[i] = (char *)0;

			// create child process and run simple commands
			execute_simple(tks, FILE, carat_in, carat_out, car_arg, amp);
		}
		

	}
}