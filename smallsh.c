/***************************************************************
 * Author: Kim McLeod
 * Filename: p3prac.c
 * Desctription: tiny shell
 * Date: 11.16.15
 * *************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // for fork(), exec(), etc. 
#include <sys/wait.h> // for wait(), waitpid(), etc.
#include <signal.h>
#include <fcntl.h> //for files

#define TOK_BUFSIZE 1024
#define TOK_DELIM "\t\r\n\a' '"

// global variables
int backgroundPID;
int backgroundStatus;
int foregroundPID;
int foregroundStatus;
int child_state;
int exStat;



// create handler to handle CTRL-C
void INThandler(int sig)
{
	
	signal(sig, SIG_IGN);
	kill(foregroundPID, sig);
	perror("terminated by signal 2");
	fflush(stdout);

}



// get input from the user
char *readInput(void)
{
	char *input = NULL;
	ssize_t bufsize; // = 0; getline gives us a buffer
	getline(&input, &bufsize, stdin);
	return input;
}

// parse the input into an array
char **splitInput(char *input)
{
	int bufsize = TOK_BUFSIZE;
	int position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;
	char *holder;
	
	if(!tokens)
	{
		fprintf(stderr, "allocation error\n"); 
		exit(EXIT_FAILURE);
	}

	token = strtok(input, TOK_DELIM);
	
	while(token != NULL)
	{
		tokens[position] = token;

		position ++;
		
		if(position >= bufsize)
		{
			bufsize += TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));

	
			if(!tokens)
			{
				fprintf(stderr, "Allocation error\n"); 
				exit(EXIT_FAILURE);
			}
		}
		



		token = strtok(NULL, TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}


// create the general signal catcher
void sigHandler(int sig)
{
	int childpid;

	switch(sig){
		
		case(SIGCHLD):
			
			// check for and manage finished children
			while((childpid = waitpid(-1, &child_state, 0)) > 0) //0
			{
				if(childpid != foregroundPID)
				{	
					printf("Background process completed: %i\n", childpid);
					fflush(stdout);

					if(WIFEXITED(child_state))
					{
						printf("Exit Status: %d\n", WEXITSTATUS(child_state));
						fflush(stdout);
					}
			
					if(WIFSIGNALED(child_state))
					{
						printf("Exit Signal: %d\n", WTERMSIG(child_state));
						fflush(stdout);
					}
		
					
					
				}

				else
				{
					if(WIFEXITED(child_state))
					{ 
						foregroundStatus =  WEXITSTATUS(child_state);
						
					}
	
					if(WIFSIGNALED(child_state))
					{
						foregroundStatus = WTERMSIG(child_state);
						
					}
		
	
				}

			}

			break;

		default:
				
			
			// or, send the signal to the child
			if(foregroundPID != 0)
			{
				kill(foregroundPID, sig);
				

			}
		}

}



int commandLaunch(char **args, int length)
{
	pid_t pid, bpid, wpid;
	int status;
	fflush(stdout);	
	signal(SIGCHLD, sigHandler);
	
	pid = fork();

	if(pid == 0) // this is the child process
	{

		
		
		// if background process
		if(*(args[length - 1]) == 38)
		{	
			signal(SIGINT, SIG_IGN);
			backgroundPID = getpid();	
			printf("Background Process Started: %i\n", backgroundPID);
			int new;
			new = open("/dev/null", O_WRONLY);
			dup2(new, 0);
			

		
			// if output needs to be changed
			if((*args[1]) == 62) // 62 = >
			{
				int fd;
				fd = open(args[2], O_WRONLY | O_TRUNC | O_CREAT, 0666);
				if(fd == -1)
				{
					printf("Could not open file\n");
					exit(1);
				}
				else
				{dup2(fd, 1);}
			}
			
	
			// if input needs to be changed
			else if((*args[1]) == 60) // <	
			{
				int fd;
				fd = open(args[2], O_RDONLY, 0777);
				if(fd == -1)
				{
					printf("Could not open file\n");
					exit(1);
				}

				else
				{dup2(fd, 0);}

			}

			else
			{
				dup2(new, 0); //in
				dup2(new, 1); //out
			}

			// strip the ampersand
			args[length - 1] = NULL;

				
		}



		// if not background process
		else{  
			if(args[1] != NULL)
			{
		
			// if output needs to be changed
			if(*(args[1]) == 62) // 62 = >
			{
			

				int fd;
				char *filename = args[2];
				
				
				fd = open(args[2], O_WRONLY | O_TRUNC | O_CREAT, 0666);
				if(fd == -1)
				{
					printf("Could not open file\n");
					fflush(NULL);
					exit(1);
				}			



				else
				{dup2(fd, 1);}

				args[1] = NULL;
				args[2] = NULL;	

				

			}
			
			// if input needs to be changed
			else if(*(args[1]) == 60) // <	
			{
				int fd = 0;
				fd = open(args[2], O_RDONLY, 0777);
				if(fd == -1)
				{
					printf("Could not open file\n");
					fflush(NULL);
					exit(1);
				}
	
				else
				{dup2(fd, 0);}
				
				
				args[1] = NULL;
				args[2] = NULL;

				

			}
			}
			
		}


		// send the command to exec!
		if(execvp(args[0], args) == -1)
		{
			perror("child process exec error\n");
		}
		perror("exec");

	}

	// if forking fails, display error
	else if(pid == -1) // > 0
	{
		perror("Error forking\n");
	}

	// in parent process
	else
	{
		// if foreground process, set pid
		if(*(args[length - 1]) != 38)
		{	
			foregroundPID = pid;
	
		}

		// if background process, set pid
		else
		{	backgroundPID = pid;
			

		}
		
	
	}


	return 1;
}






// declare built-in functions
int small_exit(char **args);
int small_cd(char **args);
int small_status(char **args);
 

//create an array to hold function names
char *built_in_strings[] = {"exit", "cd", "status"};

//create an array to hold function pointers
int(*built_in_functions[])(char**) = { &small_exit, &small_cd, &small_status };



// exit everything
int small_exit(char **args)
{
	exit(EXIT_SUCCESS);   
	return 0;
}


int small_cd(char **args)
{
	// if no argument, set to Home
	if(args[1] == NULL)
	{
		char *home;
		home = getenv("HOME");
		chdir(home);
		printf("Now in HOME directory.\n");
	}

	else
	{
		if(chdir(args[1]) != 0)
		{
			perror("smallsh");
		}
		printf("directory changed");
	}
	return 1;
}

int small_status(char **args)
{
	// return last pid and status
	printf("Completed Foreground Proccess: %i\n", foregroundPID); 
	printf("Foreground Completion Status: %i\n", foregroundStatus);
}



int commandExecute(char **args, int ARGLEN)
{

	int i;
	// if blank line, do nothing
	if(args[0] == NULL)
	{
		return 1;
	}

	// if comment, do nothing
	if(*(args[0]) == 35)
	{
		return 1;
	}

	// see if command matches built-ins
	for(i = 0; i < 3; i++)
	{
		if(strcmp(args[0], built_in_strings[i]) == 0)
		{
			return (*built_in_functions[i])(args);
		}
	}

	return commandLaunch(args, ARGLEN);
}


void smallsh_control(void)
{
	char *input;
	char **args;
	int status;
		
	// create the control loop
	do{
		fflush(stdout);
		
		int argsLength = 0;
		
		printf(": ");
		input = readInput();
		args = splitInput(input);
	
		int i;
		for(i = 0; args[i] != NULL; i++)
		{
			
			argsLength++;
		}	
		
		
		status = commandExecute(args, argsLength);

		free(input);
		free(args);
	  }while(status);

}




int main(int argc, char **argv)
{
	// set up sig handler
	signal(SIGINT, INThandler);
	struct sigaction act;
	act.sa_handler = sigHandler;
	act.sa_flags = 0;
	sigfillset(&act.sa_mask);
	sigaction(SIGCHLD, &act, NULL);

	// initialize global variables
	backgroundPID = 0;
	backgroundStatus = 0;
	foregroundPID = 0;
	foregroundStatus = 0;
	child_state = 0;
	

	smallsh_control();

	return EXIT_SUCCESS;
}

