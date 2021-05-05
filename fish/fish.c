#define _DEFAULT_SOURCE


#include "cmdline.h"
#include "fish.h"
#define BUFLEN 1024

#define YES_NO(i) ((i) ? "Y" : "N")



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
