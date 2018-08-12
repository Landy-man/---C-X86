#include "linux/limits.h"
#include "LineParser.h"
#include "JobControl.h"
#include <stdio.h>
#include <unistd.h>
#include<signal.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#define LINE_IN 2048

char *strsignal(int sig);

int debug = 0;


void sig_handler(int signo) {
    char *sig = (char *) strsignal(signo);
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
    struct termios shellattr;
    char cwd[PATH_MAX];
    char line_in[LINE_IN];
    pid_t pid;
    int i;
    job ** job_list = (job **)malloc(4);
    *job_list = NULL;
    for (i = 1; i < argc; i++) {
        if (strcmp("-d", argv[i]) == 0) {
            debug = 1;
        }
    }
    signal(SIGCHLD, sig_handler);
    signal(SIGQUIT, sig_handler);
    signal(SIGTSTP, sig_handler);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    setpgid(getpid(), getpid());


    while (1) {
        tcgetattr(STDIN_FILENO, &shellattr);

        getcwd(cwd, PATH_MAX);
        printf("%s>", cwd);
        
        memset(line_in,0,LINE_IN);
        fflush(stdin);
        fgets(line_in, LINE_IN, stdin);

        if ((strcmp("quit\n", line_in)) == 0) {
            exit(0);
        } else if ((strncmp("cd", line_in, 2)) == 0) {
            if (line_in[strlen(line_in) - 1] == '\n')
                line_in[strlen(line_in) - 1] = '\0';
            if (chdir(line_in + 3) == -1) {
                perror("CANNOT CHANGE DIRECTORY: ");
            }
        } else if ((strcmp(line_in, "jobs\n")) == 0) {
            printJobs(job_list);
        } else if ((strncmp("fg", line_in, 2)) == 0) {
            int index = atoi(&line_in[2]);
            runJobInForeground(job_list, findJobByIndex(*job_list, index), 1, &shellattr, getpgid(getpid()));
        } else if ((strncmp("bg", line_in, 2)) == 0) {
            int index = atoi(&line_in[2]);
            runJobInBackground(findJobByIndex(*job_list, index), 1);
        } else if ('\0' != line_in[0] && (strcmp("\n", line_in)) != 0) {
            
            job * current_job = addJob(job_list, line_in);
            tcgetattr(STDIN_FILENO, current_job->tmodes);
            pid = fork();
            struct cmdLine *ParsedLine = parseCmdLines(line_in);
            switch (pid) {
                
                case 0:
                    if (ParsedLine->inputRedirect!=NULL){
                        close(0);
                        open(ParsedLine->inputRedirect,O_RDONLY);
                    }
                    if (ParsedLine->outputRedirect!=NULL){
                        close(1);
                        open(ParsedLine->outputRedirect,O_RDWR+O_CREAT);
                    }
                    setpgid(getpid(), getpid());
                    
                    signal(SIGCHLD, SIG_DFL);
                    signal(SIGQUIT, SIG_DFL);
                    signal(SIGTSTP, SIG_DFL);
                    signal(SIGTTIN, SIG_DFL);
                    signal(SIGTTOU, SIG_DFL);
                    
                    execute(ParsedLine);
                    freeCmdLines(ParsedLine);
                    _exit(0);
                    break;
                    
                case -1:
                    freeCmdLines(ParsedLine);
                    _exit(1);
                    break;
                    
                default:
                    signal(SIGCHLD, sig_handler);
                    signal(SIGQUIT, sig_handler);
                    signal(SIGTSTP, sig_handler);
                    signal(SIGTTIN, SIG_IGN);
                    signal(SIGTTOU, SIG_IGN);
                    current_job->pgid = pid;
                    setpgid(pid, pid);
                    

                    if (debug) {
                        fprintf(stderr, "CHILD PID IS: %d\n", pid);
                    }
                    if (ParsedLine->blocking) {
                        runJobInForeground(job_list, current_job, 0, &shellattr, getpgid(getpid()));
                    }
                    else {
                        runJobInBackground(current_job,0);
                    }
                    freeCmdLines(ParsedLine);
                    break;
            }
        }
    }
}

