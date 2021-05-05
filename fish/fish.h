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
