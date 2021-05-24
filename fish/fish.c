#define _DEFAULT_SOURCE


#include "cmdline.h"
#include "fish.h"
#define BUFLEN 1024

#define YES_NO(i) ((i) ? "Y" : "N")

int main() {
  //Exercise 6
  //Ignore signal SIGINT for the main loop
  struct sigaction ignored;
  ignored.sa_flags = 0;
  sigemptyset(&ignored.sa_mask);
  ignored.sa_handler = SIG_IGN;
  sigaction(SIGINT, &ignored, NULL);

  //Default value to reset buffer at end of background command
  char reset_buf[] = "\n\0";


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
    If commande entered*/
    if(li.cmds->n_args != 0){
      //Execute simple commande
      exeSimpleCommand(&li);
    }

    if (li.cmds[0].args[0] == NULL) {
      line_reset(&li);
      continue;
    }

    cmd_interne(&li, chabsolu);

    if(li.background){
      strcpy(buf,reset_buf);
    }

    line_reset(&li);

  }

  return 0;
}
