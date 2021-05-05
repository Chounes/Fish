#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

void cmd_redirection(struct line li, int redirect){
    int fic;
    if(redirect == 0){
        fic = open(li.file_input, O_RDONLY);
    }else if (redirect == 1){
        fic = creat(li.file_output, S_IRWXU | S_IRWXG | S_IRWXO);
    }else{
        fprintf(stderr, "wrong redirect value");
        exit(EXIT_FAILURE);
    }
    if(fic == -1){
        perror("exec.c -> open/creat");
        exit(EXIT_FAILURE);
    }
    if(dup2(fic, redirect) == -1){
        perror("exec.c -> dup2");
        exit(EXIT_FAILURE);
    }

    if(close(fic) == -1){
        perror("exec.c -> close");
        exit(EXIT_FAILURE);
    }
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
