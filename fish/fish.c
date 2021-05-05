#define _DEFAULT_SOURCE
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

  else{
    /*Exercise 6
    Reset SIGINT to it default value just
    for execution of command*/
    struct sigaction dflt;
    dflt.sa_flags = 0;
    sigemptyset(&dflt.sa_mask);
    dflt.sa_handler = SIG_DFL;
    sigaction(SIGINT, &dflt, NULL);

    //Execute command in sub process
    if(fork()==0){
      int res = execvp(li->cmds[0].args[0],li->cmds[0].args);

      //If command badly executed, print error of command
      if(res == -1){
        perror(li->cmds[0].args[0]);
      }
      exit(1);
    }
    int stat;
    //Wait end of sub process
    int pid = wait(&stat);


    //Print pid of sub process and informations about it execution
    if(WIFEXITED(stat)){
        printf("%d exited, status=%d\n", pid, WIFSIGNALED(stat));
    }
    if(WIFSIGNALED(stat)){
        printf("%d killed by signal %d\n", pid, WTERMSIG(stat));
    }

    /*Exercise6
    Re-ignore SIGINT for the main loop*/
    dflt.sa_handler = SIG_IGN;
    sigaction(SIGINT, &dflt, NULL);
  }

  return;
}

//void handler(int sig);

//struct list pids; // list of background processes pid

int main() {
  //Exercise 6
  //Ignore signal SIGINT for the main loop
  struct sigaction ignored;
  ignored.sa_flags = 0;
  sigemptyset(&ignored.sa_mask);
  ignored.sa_handler = SIG_IGN;
  sigaction(SIGINT, &ignored, NULL);


  struct line li;
  char buf[BUFLEN];

  line_init(&li);

  char *chabsolu = getcwd(NULL, 0);
  //int **pipes = NULL;

  for (;;) {
    printf("fish> ");
    fgets(buf, BUFLEN, stdin);

    int err = line_parse(&li, buf);
    if (err) {
      //the command line entered by the user isn't valid
      line_reset(&li);
      continue;
    }

    /*Exercise 3
    Execute simple commande*/
    exeSimpleCommand(&li);

    if (li.cmds[0].args[0] == NULL) {
      line_reset(&li);
      continue;
    }

    if (strcmp(li.cmds[0].args[0], "exit") == 0) {
      line_reset(&li);
      break;
    } else if (strcmp(li.cmds[0].args[0], "cd") == 0) {
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
    }

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
