#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc,char * argv[]){
    pid_t pid1,pid2;
    int pipeLine[2],inDebug=0,status,flag;
    char *command1[3];
    command1[0]="ls";
    command1[1]="-l";
    command1[2]=NULL;
    char *command2[4];
    command2[0]="tail";
    command2[1]="-n";
    command2[2]="2";
    command2[3]=NULL;
    FILE * _out=stderr;
    
    for(flag=0;flag<argc;flag++){
    
    if(strcmp("-d",argv[flag])==0){
        
        inDebug=1;
        if (argc>2){
            _out=fopen(argv[++flag],"w+a");
            
        }
    }
    }
    
    if (pipe(pipeLine)==-1){
        perror("Error in pipe:");
        exit(1);
    }
    
    
    if(inDebug){
        fprintf(_out,"(parent_process>forking…)\n");
        
    }
    
    pid1=fork();
    
 
    
    if (pid1==0){
        if(inDebug){
            fprintf(_out,"(child1>redirecting stdout to the write end of the pipe…)\n");
        }
        close(1);
        dup(pipeLine[1]);
        close(pipeLine[1]);
        if (inDebug){
            fprintf(_out,"(child1>going to execute cmd: ls -l)\n");
        }
        execvp(command1[0],command1);
        _exit(0);
    }
    else{
        if(inDebug){
            fprintf(_out,"(parent_process>created process with id:%d )\n" , pid1); 
            fprintf(_out,"(parent_process>closing the write end of the pipe…)\n");
        }
        close(pipeLine[1]);
    }
    pid2=fork();
    
    if (pid2==0){
        if(inDebug){
            fprintf(_out,"(child2>redirecting stdin to the read end of the pipe…)\n");
        }
        close(0);
        dup(pipeLine[0]);
        close(pipeLine[0]);
        if (inDebug){
            fprintf(_out,"(child2>going to execute cmd: tail -n 2)\n");
        }
        execvp(command2[0],command2);
    }
    else{
        if(inDebug){
            fprintf(_out,"(parent_process>closing the read end of the pipe…)\n");
        }
        close(pipeLine[0]);
    }
    if(inDebug){
        fprintf(_out,"(parent_process>waiting for child processes to terminate…)\n");
    }
    waitpid(pid1,&status,WUNTRACED);
    if(inDebug){
        fprintf(_out,"(parent_process>waiting for child processes to terminate…)\n");
    }
    waitpid(pid2,&status,WUNTRACED);
    if(inDebug){
        fprintf(_out,"(parent_process>exiting…)\n");
    }
    fclose(_out);
    return 0;
    
}