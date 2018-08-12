#include "LineParser.h"
#include "linux/limits.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>


#define LINE_IN 2048


int debug = 0;



void sig_handler(int signo) {
    char *sig = (char *)strsignal(signo);
    printf("\n%s signal was ignored\n", sig);
}

void execute(cmdLine *pCmdLine) {
        if (debug) {
            fprintf(stderr, "COMMAND IS: %s\n", pCmdLine->arguments[0]);
        }
        int i = execvp(pCmdLine->arguments[0], pCmdLine->arguments);
        if (i == -1) {
            perror("ERROR: ");
            _exit(1);
        }
}


int main(int argc, char *argv[]) {
    int status = 0;
    pid_t pid;
    char cwd[PATH_MAX];
    char line_in[LINE_IN];
    int i = 0;
    for (i = 1; i < argc; i++) {
        if (strcmp("-d", argv[i]) == 0) {
            debug = 1;
        }
    }
    
    

        
        
    while (1) {
        signal(SIGCHLD, sig_handler);
        signal(SIGQUIT, sig_handler);
        signal(SIGTSTP, sig_handler);
        getcwd(cwd, PATH_MAX);
        printf("%s >", cwd);
        line_in[0] = '\0';
        fflush(stdin);
        fgets(line_in, LINE_IN, stdin);
        
        if ((strcmp("quit\n", line_in)) == 0) {
            exit(0);
        }

        else if ((strncmp("cd", line_in, 2)) == 0) {
            if (line_in[strlen (line_in) - 1] == '\n')
                line_in[strlen (line_in) - 1] = '\0';
            if(chdir(line_in + 3) == -1){
                perror("CANNOT CHANGE DIRECTORY: ");

            }
        }

        else if ('\0' != line_in[0] && (strcmp("\n" , line_in)) != 0) {
            cmdLine *ParsedLine = parseCmdLines(line_in);
                pid = fork();
                switch (pid) {
                    case 0:
                        execute(ParsedLine);
                        _exit(0);
                        break;
                    case -1:
                        exit(1);
                        break;
                    default:
                        if (debug) {
                            fprintf(stderr, "CHILD PID IS: %d\n", pid);
                        }
                        if (ParsedLine->blocking) {
                            waitpid(pid, &status, 0);
                        }
                        usleep(100000);
                        break;
                }    
        }
    }
}

