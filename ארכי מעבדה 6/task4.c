#include "linux/limits.h"
#include "LineParser.h"
#include "JobControl.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#define LINE_IN 2048
char *strsignal(int sig);

struct node {
   char * data;
   char * key;
   struct node * next;
};




/*List implemntation*/

struct node *head = NULL;
struct node *current = NULL;

//display the list
void printList() {
   struct node *ptr = head;
   printf("\n[ ");
	
   //start from the beginning
   while(ptr != NULL) {
      printf("(%s,%s) ",ptr->key,ptr->data);
      ptr = ptr->next;
   }
	
   printf(" ]");
}

//find a link with given key
struct node* find(char * key) {

   //start from the first link
   struct node* current = head;

   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //navigate through list
   while(strcmp(current->key, key)!=0) {
	
      //if it is last node
      if(current->next == NULL) {
         return NULL;
      } else {
         //go to next link
         current = current->next;
      }
   }      
	
   //if data found, return the current Link
   return current;
}

//insert link at the first location
void insertFirst(char * key, char * data) {
   if ((current=find(key))==NULL){
    
    //create a link
   struct node *link = (struct node*) malloc(sizeof(struct node));
	
   link->key = key;
   link->data = data;
	
   //point it to old first node
   link->next = head;
	
   //point first to new first node
   head = link;
   }
   else{
        free(current->data);
        current->data=data;
   }
   
}

//delete first item
struct node* deleteFirst() {

   //save reference to first link
   struct node *tempLink = head;
	
   //mark next to first link as first 
   head = head->next;
	
   //return the deleted link
   return tempLink;
}

//is list empty
int isEmpty() {
   return head == NULL;
}

int length() {
   int length = 0;
   struct node *current;
	
   for(current = head; current != NULL; current = current->next) {
      length++;
   }
	
   return length;
}



//delete a link with given key
struct node* delete(char * key) {

   //start from the first link
   struct node* current = head;
   struct node* previous = NULL;
	
   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //navigate through list
   while(strcmp(current->key , key)) {

      //if it is last node
      if(current->next == NULL) {
         return NULL;
      } else {
         //store reference to current link
         previous = current;
         //move to next link
         current = current->next;
      }
   }

   //found a match, update the link
   if(current == head) {
      //change first to point to next link
      head = head->next;
   } else {
      //bypass the current link
      previous->next = current->next;
   }    
	
   return current;
}

/*End of list implemntation*/





int debug = 0;

char * HomeEnv ="~";

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

int eval (cmdLine *pCmdLine){
    int i=0;
    while (pCmdLine->arguments[i]!=NULL){
        if (strncmp(pCmdLine->arguments[i],"$",1)==0){
            struct node *currentArgumentToSwitch=find(pCmdLine->arguments[i]+1);
            if (currentArgumentToSwitch==NULL){
                perror("Variable was not found\n");
                return 1;
            }
            else{
                replaceCmdArg(pCmdLine,i,currentArgumentToSwitch->data);
            }
                
        }
        i++;
    }
    return 0;
}


int main(int argc, char *argv[]) {
    struct termios shellattr;
    char cwd[PATH_MAX];
    char line_in[LINE_IN];
    pid_t pid,pid2;
    int i,pipeLine[2],status;
    job ** job_list = (job **)malloc(4);
    *job_list = NULL;
    for (i = 1; i < argc; i++) {
        if (strcmp("-d", argv[i]) == 0) {
            debug = 1;
        }
    }
    char * homeDir =getenv("HOME");
    
    
    signal(SIGCHLD, sig_handler);
    signal(SIGQUIT, sig_handler);
    signal(SIGTSTP, sig_handler);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    setpgid(getpid(), getpid());
    
    if (pipe(pipeLine)==-1){
        perror("Error in pipe:");
        exit(1);
    }


    while (1) {
        tcgetattr(STDIN_FILENO, &shellattr);

        getcwd(cwd, PATH_MAX);
        printf("%s>", cwd);
        
        memset(line_in,0,LINE_IN);
        fflush(stdin);
        fgets(line_in, LINE_IN, stdin);

        if ((strcmp("quit\n", line_in)) == 0) {
            freeJobList(job_list);
            free(job_list);
            exit(0);
        } else if ((strncmp("cd", line_in, 2)) == 0) {

            if (line_in[strlen(line_in) - 1] == '\n')
                line_in[strlen(line_in) - 1] = '\0';
            struct cmdLine *ParsedLine = parseCmdLines(line_in);
            if (strncmp(ParsedLine->arguments[1],"~",1)==0){
                replaceCmdArg(ParsedLine,1,homeDir);
                chdir(ParsedLine->arguments[1]);
            }
            else  {
                if ((chdir(line_in + 3) == -1))
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
        }else if ((strncmp("set", line_in, 3)) == 0) {
            struct cmdLine *ParsedLine = parseCmdLines(line_in);
            insertFirst(ParsedLine->arguments[1],ParsedLine->arguments[2]);
        }else if ((strncmp("env", line_in, 3)) == 0) {
            printList();
            printf("\n");
        }else if ((strncmp("delete", line_in, 6)) == 0) {
            struct cmdLine *ParsedLine = parseCmdLines(line_in);
            if (find(ParsedLine->arguments[1])==NULL)
                perror("Variable not found\n");
            else
                delete(ParsedLine->arguments[1]);
            
        }else if ('\0' != line_in[0] && (strcmp("\n", line_in)) != 0) {
            
            job * current_job = addJob(job_list, line_in);
            tcgetattr(STDIN_FILENO, current_job->tmodes);
            pid = fork();
            struct cmdLine *ParsedLine = parseCmdLines(line_in);
            eval(ParsedLine);
            switch (pid) {
                
                case 0:
                    if (ParsedLine->inputRedirect!=NULL){
                        close(0);
                        open(ParsedLine->inputRedirect,O_RDONLY);
                    }
                    if (ParsedLine->outputRedirect!=NULL){
                        close(1);
                        open(ParsedLine->outputRedirect,O_RDWR|O_CREAT|O_APPEND, 0640);
                    }
                    if (ParsedLine->next!=NULL){
                        close(1);
                        dup(pipeLine[1]);
                        close(pipeLine[1]);
                        if (debug){
                            fprintf(stderr,"(child1>going to execute cmd: ls -l)\n");
                        }
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
                    if (ParsedLine->next!=NULL){
                        eval(ParsedLine->next);
                        close(pipeLine[1]);
                        pid2=fork();
    
                        if (pid2==0){
                            if (ParsedLine->next->inputRedirect!=NULL){
                                close(0);
                                open(ParsedLine->next->inputRedirect,O_RDONLY);
                            }
                            if (ParsedLine->next->outputRedirect!=NULL){
                                close(1);
                                open(ParsedLine->next->outputRedirect,O_RDWR|O_CREAT|O_APPEND , 0640);
                            }
                            
                            signal(SIGCHLD, SIG_DFL);
                            signal(SIGQUIT, SIG_DFL);
                            signal(SIGTSTP, SIG_DFL);
                            signal(SIGTTIN, SIG_DFL);
                            signal(SIGTTOU, SIG_DFL);
                            setpgid(pid2, pid);
                            if(debug)
                                fprintf(stderr,"(child2>redirecting stdin to the read end of the pipe…)\n");
                        
                            close(0);
                            dup(pipeLine[0]);
                            close(pipeLine[0]);
                            if (debug)
                                fprintf(stderr,"(child2>going to execute cmd: tail -n 2)\n");
                            
                            execvp(ParsedLine->next->arguments[0],ParsedLine->next->arguments);
                            _exit(0);
                            break;
                        }
                        else{
                            if(debug)
                                fprintf(stderr,"(parent_process>closing the read end of the pipe…)\n");
                        
                            close(pipeLine[0]);
                            if(debug)
                                fprintf(stderr,"(parent_process>waiting for child processes to terminate…)\n");
                            
                            waitpid(pid,&status,WUNTRACED);
                            if(debug)
                                fprintf(stderr,"(parent_process>waiting for child processes to terminate…)\n");
                            waitpid(pid2,&status,WUNTRACED);
                            }
                        }
                    
                    signal(SIGCHLD, sig_handler);
                    signal(SIGQUIT, sig_handler);
                    signal(SIGTSTP, sig_handler);
                    signal(SIGTTIN, SIG_IGN);
                    signal(SIGTTOU, SIG_IGN);
                    current_job->pgid = pid;
                    setpgid(pid, pid);
                    

                    if (debug)
                        fprintf(stderr, "CHILD PID IS: %d\n", pid);
                    if (ParsedLine->blocking) 
                        runJobInForeground(job_list, current_job, 0, &shellattr, getpgid(getpid()));
                    
                    else 
                        runJobInBackground(current_job,0);
                    
                    freeCmdLines(ParsedLine);
                    break;
                    }
        }
    }
}

