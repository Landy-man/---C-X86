#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*structs and variables*/
typedef struct virus virus;
 
struct virus {
    unsigned short length;
    char name[16];
    char signature[];
};


typedef struct link link;
 
struct link {
    virus *v;
    link *next;
};

int _appendToEnd=1;
int _listLength=0;


/*pre-defined functions*/


void PrintHex (char * fileStream,int length) {
/*Prints the hexa represention*/
    int _counter=0;
    
    while (_counter<length-1){
       printf("%02X " ,(unsigned char)fileStream[_counter]);
       _counter++;
    }
    if (length>0)
        printf("%02X\n" ,(unsigned char)fileStream[_counter]);
    
}


void toString(virus * _outputVir){
/*prints a virus in the desired format*/
    printf("Virus name: %s\n",_outputVir->name);
    printf("Virus size: %d\n",_outputVir->length);
    printf("signature:\n");
    PrintHex(_outputVir->signature,_outputVir->length);
    printf("\n");
}


/*initilize virus name and signature*/
void VirusInit(virus * _unInitVir,int length,FILE * _from){
    fread(_unInitVir->name,1,length,_from);
}

int getlen(char _isBigIndi,FILE * _fileStream){
    char * _input;
    int _length;
    _input=calloc(2,sizeof(char));
    fread(_input,1,2,_fileStream);
    if (_isBigIndi==0){
        _length=(_input[0]+_input[1]*256)-18;
    }else{
         _length=(_input[1]+_input[0]*256)-18;
    }
    free(_input);
    return _length;
}

void list_print(link *virus_list){
/* Print the data of every link in list. Each item followed by a newline character. */
     if (virus_list==NULL)
        return;
     else{
        toString(virus_list->v);
        list_print(virus_list->next);
     }
}

link* list_append(link* virus_list, virus* data){
     /* Add a new link with the given data to the list 
        (either at the end or the beginning, depending on what your TA tells you),
        and return a pointer to the list (i.e., the first link in the list).
        If the list is null - create a new entry and return a pointer to the entry. */
    link * _newLink=malloc(8);
    _newLink->v=data;
    _newLink->next=NULL;
    link * list_link=virus_list;
    _listLength++;
    if(virus_list!=NULL){        
        if(_appendToEnd==0){
            _newLink->next=virus_list;
        }else{
            while(list_link->next!=NULL){
                list_link=list_link->next;
            }
            list_link->next=_newLink;
            return virus_list;
        }
    }            
    return _newLink;
}


void list_free(link *virus_list){
/* Free the memory allocated by the list. */
        if (virus_list->next==NULL){
                free(virus_list->v);
        }
        else{
                list_free(virus_list->next);
                free(virus_list->v);
        }
        free(virus_list);
}



void detect_virus(char * buffer, link *virus_list, unsigned int size){
    unsigned int _counter;
    int _flagAlert;
    link * _temp;
    for(_counter=0;_counter<size;_counter++){
        _temp=virus_list;
        while(_temp!=NULL){
            if (size-_counter>=_temp->v->length){
                _flagAlert=memcmp(buffer+_counter,_temp->v->signature,_temp->v->length);
                if(_flagAlert==0){
                    printf("Infected position starts at: %d\n",_counter);
                    printf("Virus name: %s\n",_temp->v->name);
                    printf("Virus size is: %d\n",_temp->v->length);
                    printf("\n");
                }
            }
            _temp=_temp->next;
        }
        
    }
    
}


/*****************************************
******************************************



        Main function



******************************************
*****************************************/

int main(int argc, char **argv) {
    /*Variables*/
    FILE * _virus_signature = fopen("sig","r");
    FILE * _suspectedFile=NULL;
    char * buffer=malloc((1<<10)*10);
    virus * _vir=NULL;
    link * virus_list=NULL;
    
    int size=0, _singleVirSize=0;
    unsigned int _sizeOfFile=0;
    char _isBigIndi;
    
    
    if(argc!=2||((_suspectedFile=fopen(argv[1],"r"))==0)){
        printf("404. can not find Suspected File. Exitting\n");
        return 1;
        
    }
    
    
    
        
        
    /*Logic*/
    /* getting file length*/
    fseek(_virus_signature, 0, SEEK_END);
    size= ftell(_virus_signature);
    rewind(_virus_signature);
    size--;
    
     _isBigIndi=fgetc(_virus_signature);
     
    while(size>0){
    
   
    
    /*Virus init*/
        _singleVirSize=getlen(_isBigIndi,_virus_signature);
        _vir=malloc(_singleVirSize+18);
        _vir->length=_singleVirSize;
        VirusInit(_vir,_vir->length+16,_virus_signature);
        
    /*appending virus to list*/
        virus_list=list_append(virus_list,_vir);
        
        size=size-(_singleVirSize+18);
    }
    
    /*
    if((_suspectedFile=fopen(,"r"))==0){
        printf("404. File was not found! exiting\n);
        return 1;
    }
    */
    _sizeOfFile=fread(buffer,1,(1<<10)*10,_suspectedFile);
    detect_virus(buffer,virus_list,_sizeOfFile);
    
    /*
    list_print(virus_list);
    printf("List size is: %d\n",_listLength);
    
    */
    fclose(_suspectedFile);
    fclose(_virus_signature);
    list_free(virus_list);
    free(buffer);
    return 0;
}

