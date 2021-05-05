#include "cmdline.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define INPUT_REDIRECT 0
#define OUTPUT_REDIRECT 1

char *chabsolu = getcwd(NULL, 0);

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
			continue;
		}
		free(chabsolu);
		chabsolu = getcwd(NULL, 0);
		return 1;
	}

	return 0;
}
