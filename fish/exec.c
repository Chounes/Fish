#include "exec.h"

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
        perror("cmdexec.c -> open/creat");
        exit(EXIT_FAILURE);
    }
    if(dup2(fic, redirect) == -1){
        perror("cmdexec.c -> dup2");
        exit(EXIT_FAILURE);
    }

    if(close(fic) == -1){
        perror("cmdexex.c -> close");
        exit(EXIT_FAILURE);
    }
}
