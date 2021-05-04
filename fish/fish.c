
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "cmdline.h"

#define BUFLEN 1024

#define YES_NO(i) ((i) ? "Y" : "N")


//Exercise 3
//Execute basic commands
void exeSimpleCommand(struct line *li){

  //If command have'nt any arguments
  if(li->cmds->n_args <1){
    fprintf(stderr,"Error : please enter a command\n");
    fprintf(stderr,"Usage : [command] [options]\n");
  }
  else{
    if(fork()==0){
      int res = execvp(li->cmds[0].args[0],li->cmds[0].args);

      //If command badly executed, print error of command
      if(res == -1){
        perror(li->cmds[0].args[0]);
      }
      exit(1);
    }
    int stat;
    int pid = wait(&stat);

    if(WIFEXITED(stat)){
        printf("%d exited, status=%d\n", pid, WIFSIGNALED(stat));
    }
    if(WIFSIGNALED(stat)){
        printf("%d killed by signal %d\n", pid, WTERMSIG(stat));
    }
  }
  return;
}


int main() {
  struct line li;
  char buf[BUFLEN];

  line_init(&li);

  for (;;) {
    printf("fish> ");
    fgets(buf, BUFLEN, stdin);
    int err = line_parse(&li, buf);
    if (err) {
      //the command line entered by the user isn't valid
      line_reset(&li);
      continue;
    }

    exeSimpleCommand(&li);

    fprintf(stderr, "Command line:\n");
    fprintf(stderr, "\tNumber of commands: %zu\n", li.n_cmds);

    for (size_t i = 0; i < li.n_cmds; ++i) {
      fprintf(stderr, "\t\tCommand #%zu:\n", i);
      fprintf(stderr, "\t\t\tNumber of args: %zu\n", li.cmds[i].n_args);
      fprintf(stderr, "\t\t\tArgs:");
      for (size_t j = 0; j < li.cmds[i].n_args; ++j) {
        fprintf(stderr, " \"%s\"", li.cmds[i].args[j]);
      }
      fprintf(stderr, "\n");
    }

    fprintf(stderr, "\tRedirection of input: %s\n", YES_NO(li.redirect_input));
    if (li.redirect_input) {
      fprintf(stderr, "\t\tFilename: '%s'\n", li.file_input);
    }

    fprintf(stderr, "\tRedirection of output: %s\n", YES_NO(li.redirect_output));
    if (li.redirect_output) {
      fprintf(stderr, "\t\tFilename: '%s'\n", li.file_output);
    }

    fprintf(stderr, "\tBackground: %s\n", YES_NO(li.background));

    /* do something with li */

    line_reset(&li);
  }

  return 0;
}
