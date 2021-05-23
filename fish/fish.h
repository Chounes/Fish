#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#define INPUT_REDIRECT 0
#define OUTPUT_REDIRECT 1

//Handler used for background process
void handSIG_CHILD(int signal){
	int stat;
	//Wait end of sub process
	int pid = wait(&stat);

	//Print pid of sub process and informations about it execution
	if(WIFEXITED(stat)){
			printf("\tBG : %d exited, status=%d\n", pid, WIFSIGNALED(stat));
	}
	if(WIFSIGNALED(stat)){
			printf("\tBG : %d killed by signal %d\n", pid, WTERMSIG(stat));
	}
}

//Handle input and output redirection. Return the new file descriptor if succeeded, -1 if not
int cmd_redirection(const char *file, int type)
{
	//NULL file
	if(!file)
	{
		fprintf(stderr, "Error : Cannot redirect to a NULL file.\n");
		return -1 ;
	}

	printf("Redirecting %s to file '%s'.\n", (type == INPUT_REDIRECT) ? "input" : "output", file);

	//Wrong type
	if(type != INPUT_REDIRECT && type != OUTPUT_REDIRECT)
	{
		fprintf(stderr, "Error : wrong redirection type in source code.\n");
		return -1 ;
	}

	//opening file
	int flags = (type == INPUT_REDIRECT) ? O_RDONLY : (O_WRONLY | O_CREAT);
	int fd = open(file, flags);
	if(fd < 0)
	{
		fprintf(stderr, "Unable to open file '%s'\n",file);
		return -1;
	}

	//Redirecting
	int dup_fd = (type == INPUT_REDIRECT) ? STDIN_FILENO : STDOUT_FILENO;
	if(dup2(fd, dup_fd) < 0)
	{
		fprintf(stderr, "Unable to redirect %s\n", (type == INPUT_REDIRECT) ? "input" : "output");
		return -1;
	}

	close(fd);

	return 0;
}

//Execute background commands
void backgroundCommand(struct line *li, int numCommand){
	//Exercise 7
	//redirect signal for end of child to print status
	struct sigaction child;
	child.sa_flags = 0;
	sigemptyset(&child.sa_mask);
	child.sa_handler = handSIG_CHILD;
	sigaction(SIGCHLD, &child, NULL);

	if (fork() == 0){
		int res;
		//If redirection for input
		if(li->redirect_input){
			res = cmd_redirection(li->file_input, 0);
			//res == -1 redirection failed
			if(res == -1) exit(EXIT_FAILURE);
		}

		//If redirection for output
		if(li->redirect_output){
			res = cmd_redirection(li->file_output, 1);
			//res == -1 redirection failed
			if(res == -1) exit(EXIT_FAILURE);
		}

		res = execvp(li->cmds[numCommand].args[0],li->cmds[numCommand].args);

		//If command badly executed, print error of command
		if(res == -1){
			perror(li->cmds[numCommand].args[0]);
		}
		exit(EXIT_SUCCESS);
	}
	return;
}

//Execute foreground commands
void foregroundCommand(struct line *li){
	//reset signal of end of child to not wait background command in case of foreground command
	struct sigaction child;
	child.sa_flags = 0;
	sigemptyset(&child.sa_mask);
	child.sa_handler = SIG_IGN;
	sigaction(SIGCHLD, &child, NULL);

	int fg_pid;
	int stat;

	//Execute command in sub process
	if((fg_pid = fork())==0){
		/*Exercise 6
		Reset SIGINT to it default value just
		for execution of command*/
		struct sigaction dflt;
		dflt.sa_flags = 0;
		sigemptyset(&dflt.sa_mask);
		dflt.sa_handler = SIG_DFL;
		sigaction(SIGINT, &dflt, NULL);

		int res;
		//If redirection for input
		if(li->redirect_input){
			res = cmd_redirection(li->file_input, 0);
			//res == -1 redirection failed
			if(res == -1) exit(EXIT_FAILURE);
		}

		//If redirection for output
		if(li->redirect_output){
			res = cmd_redirection(li->file_output, 1);
			//res == -1 redirection failed
			if(res == -1) exit(EXIT_FAILURE);
		}

		res = execvp(li->cmds[0].args[0],li->cmds[0].args);

		//If command badly executed, print error of command
		if(res == -1){
			perror(li->cmds[0].args[0]);
		}
		exit(EXIT_SUCCESS);
	}
	//Wait end of sub process
	waitpid(fg_pid, &stat, 0);

	//Print pid of sub process and informations about it execution
	if(WIFEXITED(stat)){
		printf("\tFG : %d exited, status=%d\n", fg_pid, WIFSIGNALED(stat));
	}
	if(WIFSIGNALED(stat)){
		printf("\tFG : %d killed by signal %d\n", fg_pid, WTERMSIG(stat));
	}
	return;
}

/*Exercise 3
Execute basic commands*/
void exeSimpleCommand(struct line *li){

	//If command have'nt any arguments
	if(li->cmds->n_args <1){
		fprintf(stderr,"Error : please enter a command\n");
		fprintf(stderr,"Usage : [command] [options]\n");
	}

	//Don't execute command if command start by "exit" or "cd"
	else if(strcmp(li->cmds[0].args[0],"exit") == 0 || strcmp(li->cmds[0].args[0],"cd") == 0);

	//backgroundCommand
	else if(li->background){
		backgroundCommand(li, 0);
	}
	else{
		foregroundCommand(li);
	}
	
	return;
}



//Handle internal commands like cd or exit. Return 1 if an internal command other than exit has been executed, -1 if more than 1 commands, 0 if no internal commands.
int cmd_interne(struct line li)
{
	if(li.n_cmds > 1)
	{
		return -1;
	}

	//exit command
	if(strcmp(li.cmds[0].args[0],"exit") == 0)
	{
		printf("exiting...\n");
		line_reset(&li);
		exit(EXIT_SUCCESS);
	}

	//cd command
	if(strcmp(li.cmds[0].args[0],"cd") == 0)
	{
		size_t len_dir = strlen("home") + strlen(getenv("USER")) + 1;
		char dir[len_dir];
		for (size_t i = 0; i < len_dir; ++i) {
			dir[i] = '\0';
		}
		if (li.cmds[0].n_args < 2) {
		  	strcpy(dir, "~");
		} else {
			strcpy(dir, li.cmds[0].args[1]);
		}
		if (strcmp(dir, "~") == 0) {
			char *user = getenv("USER");
			strcpy(dir, "/home/");
			strcat(dir, user);
		}
		if (chdir(dir) == -1) {
			perror("chdir");
			fprintf(stderr, "failed to change directory to %s\n", dir);
			line_reset(&li);
		}
		return 1;
	}

	return 0;
}

//Handle a command line with pipes
void handle_with_pipes(struct line li, struct sigaction ignored){
	struct sigaction child;
	child.sa_flags = 0;
	sigemptyset(&child.sa_mask);
	child.sa_handler = SIG_IGN;
	sigaction(SIGCHLD, &child, NULL);
	
	if(cmd_interne(li) != -1)
	{
		return ;
	}

	int pipes[li.n_cmds - 1][2];
	for(size_t i=0; i<li.n_cmds; i++)
	{
		if(i != li.n_cmds - 1)
		{
			pipe(pipes[i]);
		}

		if(fork() == 0)
		{
			if(i == 0)//first comand
			{
				//Output of process to input of first pipe
				dup2(pipes[0][1],STDOUT_FILENO);
			}
			else if (i  == li.n_cmds - 1)//last command
			{
				//Output of last pipe to input of process
				dup2(pipes[i-1][0],STDIN_FILENO);
			}
			else//others commands
			{
				//Output of previous pipe to input of process
				dup2(pipes[i-1][0],STDIN_FILENO);
				//Output of process to input of next pipe
				dup2(pipes[i][1],STDOUT_FILENO);
			}
			
			//closing pipe in child
			close(pipes[i][1]);
			close(pipes[i][0]);

			/*------------------------------------------------
								EXECUTION
			------------------------------------------------*/
			execvp(li.cmds[i].args[0],li.cmds[i].args);
			fprintf(stderr,"Unknown command '%s'\n",li.cmds[i].args[0]);
			exit(EXIT_FAILURE);
		}
	}
	
	for(size_t i=0; i<li.n_cmds-1; i++)//closing pipes in father
	{
		close(pipes[i][1]);
		close(pipes[i][0]);
	}
	
	for(size_t i=0; i<li.n_cmds; i++)//waiting for all commands
	{
		wait(NULL);
	}
}
