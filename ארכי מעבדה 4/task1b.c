#include "util.h"

#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_SEEK 19

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDRW 2
#define O_CREAT 64
#define O_TRUNC 512


#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define STDOUT 1
#define STDIN 0
#define STDERR 2

extern int system_call();

int _inDebug = 0;

int readWrite(int OP_CODE,int _file, char * buffer,int _size){
    int _ans=system_call(OP_CODE,_file,buffer,_size);
    if (_inDebug==0)
        return _ans;
    else{
        system_call(SYS_WRITE,STDERR,"\n",1);
        system_call(SYS_WRITE,STDERR,"OP Code: ",9);
        system_call(SYS_WRITE,STDERR,itoa(OP_CODE),1);
        system_call(SYS_WRITE,STDERR," Returned: ",11);
        system_call(SYS_WRITE,STDERR,itoa(_ans),1);
        system_call(SYS_WRITE,STDERR,"\n",1);
        return _ans;
    }
}

int openCall(int OP_CODE,char * _path, int access,int permission){
    int _ans=system_call(OP_CODE,_path,access,permission);
    if (_inDebug==0)
        return _ans;
    else{
        system_call(SYS_WRITE,STDERR,"\n",1);
        system_call(SYS_WRITE,STDERR,"OP Code: ",9);
        system_call(SYS_WRITE,STDERR,itoa(OP_CODE),3);
        system_call(SYS_WRITE,STDERR," Returned: ",11);
        system_call(SYS_WRITE,STDERR,itoa(_ans),1);
        system_call(SYS_WRITE,STDERR,"\n",1);
        return _ans;
    }
}


int main (int argc , char* argv[], char* envp[])
{
    char _singleChar= 0;
    int _fileStreamInput=0,_fileStreamOutput=1,i;
    char * _inputName, *_outputName;
        
    for (i=1;i<argc;i++){
	/*checking for "-i FileName" struct*/  
	if (strcmp(argv[i],"-i")==0){
            _inputName=argv[i+1];
		_fileStreamInput=openCall(SYS_OPEN,_inputName,O_RDONLY,0777);
		i++; /*jumping i over the file name in order not to mix with the key*/
	}
	/*Checking whether to print to screen or file*/
	else if(strcmp(argv[i],"-o")==0){
            _outputName=argv[i+1];
            _fileStreamOutput=openCall(SYS_OPEN,_outputName,O_WRONLY+O_CREAT+O_TRUNC,0777);
            i++; /*jumping the i over the output file name */
        }
        else if (strcmp(argv[i],"-d")==0) /* checks if we are in debug*/
            _inDebug=1;
    }
    
    if (_fileStreamInput<0 || _fileStreamOutput<0){
        readWrite(SYS_WRITE,STDOUT,"Unalble to open file\n",21);
        return 0x55;
        
    }
    if (_inDebug){
        system_call(SYS_WRITE,STDERR," arguments \n", 12);
        for (i = 0 ; i < argc ; i++){
            system_call(SYS_WRITE,STDERR,"argv[", 5);
            
            system_call(SYS_WRITE,STDERR,itoa(i),strlen(itoa(i)));
            system_call(SYS_WRITE,STDERR,"] = ",4);
            system_call(SYS_WRITE,STDERR,argv[i],strlen(argv[i]));
            system_call(SYS_WRITE,STDERR,"\n",1);
        }
        if (_fileStreamInput==0)
            system_call(SYS_WRITE,STDERR,"input path: stdin\n",18);
        else{
            system_call(SYS_WRITE,STDERR,"input path: .",13);
            system_call(SYS_WRITE,STDERR,_inputName,strlen(_inputName));
            system_call(SYS_WRITE,STDERR,"\n",1);
            
        }
        if (_fileStreamOutput==1)
            system_call(SYS_WRITE,STDERR,"output path: stdout\n",20);
        else{
            system_call(SYS_WRITE,STDERR,"output path: .",14);
            system_call(SYS_WRITE,STDERR,_outputName,strlen(_outputName));
            system_call(SYS_WRITE,STDERR,"\n",1);
        
    }
    }
        

    do{
    if (readWrite(SYS_READ,_fileStreamInput,&_singleChar,1)<0)
        break;

    if (_singleChar>='A' && _singleChar<='Z')
        _singleChar=_singleChar-'A'+'a';
    readWrite(SYS_WRITE,_fileStreamOutput,&_singleChar,1);
    }while(_singleChar!='\n');
    
    
    
    
    return 0;
}