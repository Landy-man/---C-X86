#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MaxSize 100
#define Hex 1
#define Dec 0



int _inDebug = 0;
int _size = 1;
char _fileName[MaxSize]="";
char * _readUnits;

/***********************void Fun()******************************/
void toggleDebug(){
    if(!_inDebug)
        printf("Debug flag now on\n");
    else
        printf("Debug flag now off\n");

    _inDebug=(_inDebug+1)%2;
}

void setFileName(){
    char c;
    printf("Please enter a file name:\n");
    while ((c = getchar()) != '\n' && c != EOF) { }
    fgets(_fileName,MaxSize,stdin);
    if(_fileName[strlen(_fileName)-1]=='\n')
        _fileName[strlen(_fileName)-1]=0;
    if(_inDebug)
        printf("Debug: file name set to %s\n",_fileName);
}

void setUnitSize(){
    char c;
    int _changedValue;
    printf("Please enter the unit size\n");
    while ((c = getchar()) != '\n' && c != EOF) { }
    scanf("%d",&_changedValue);
    if (_changedValue==1 || _changedValue==2 || _changedValue==4){
        _size=_changedValue;
        if(_inDebug)
            printf("Debug: set size to %d\n",_size);
        return;
    }
    perror("Invalid unit:");
}

void quitProg(){
    printf("quitting\n");
    fflush(stdout);
    exit(0);
}

void printBuff(char * buffer,int length ,int mode, int location){
    int i,j,sum;
    if (mode==Hex){
        for (i = 0; i < length*_size; i++) {
            if(i%16==0){
                printf("\n");
                printf("0x%x: ", location + i);
                
            }
            for (j = _size-1; j >= 0; j--) {
                printf("%02X",(unsigned char)buffer[i+j]);
            }
            i += _size-1;
            printf(" ");

    }
    }else if (mode == Dec){
        for (i = 0; i < length*_size; i++) {
            sum =0;
            for (j = _size-1; j >= 0; j--) {
                sum=sum*256+(unsigned char)buffer[i+j];
            }
            printf("%u",sum);
            i += _size-1;
            printf(" ");
        }
    }else
        return;
    printf("\n");
}

void fileDisplay(){
    FILE * _fileToDisplay=NULL;
    unsigned int _location,_length;
    

    if(strlen(&_fileName)==0){
        printf("Error: No file name\n");
        return;
    }
    if((_fileToDisplay=fopen(&_fileName,"r+"))==NULL){
        printf("Error: Unable to open file\n");
        return;
    }
    if(_readUnits!=NULL)
        free(_readUnits);
    printf("Please enter <location> <length>\n");
    scanf("%x",&_location);
    scanf("%d",&_length);
    if(_inDebug)
        printf("Location:%x\tLength:%d\n",_location,_length);
    _readUnits=(char *)malloc(_size*_length);
    fseek(_fileToDisplay,_location,SEEK_SET);
    fread(_readUnits,1,_length*_size,_fileToDisplay);
    fclose(_fileToDisplay);
    printf("Hexadecimal Representation:\n");
    printBuff(_readUnits,_length,Hex,_location);
    /*
    printf("Decimal Representation:\n");
    printBuff(_readUnits,_length,Dec);
    */
    
}

void loadToMem(){
    FILE * _fileToDisplay=NULL;
    unsigned int _location,_length;
    if(strlen(&_fileName)==0){
        printf("Error: No file name\n");
        return;
    }
    if((_fileToDisplay=fopen(&_fileName,"r+"))==NULL){
        printf("Error: Unable to open file\n");
        return;
    }
    printf("Please enter <location> <length>\n");
    scanf("%x",&_location);
    scanf("%d",&_length);
    if(_readUnits!=NULL)
        free(_readUnits);
    _readUnits=(char *)malloc(_length);
    fseek(_fileToDisplay,_location,SEEK_SET);
    fread(_readUnits,1,_length,_fileToDisplay);
    fclose(_fileToDisplay);
        if(_inDebug){
        printf("File name: %s\tBuffer pointer: %p (%02x %02x %02x %02x)\tLocation:%x\tLength:%d\n",_fileName,&_readUnits,*(_readUnits),*(_readUnits + 1),*(_readUnits + 2),*(_readUnits + 3),_location,_length);
    }
    printf("Loaded %d bytes into 0x%x\n",_length,_readUnits);
}

void saveToFile(){
    FILE * _fileToDisplay=NULL;
    unsigned int _location,_length;
    char * _sourceAdd;


    printf("Please enter <source-address> <target-location> <length>\n");
    scanf("%x",&_sourceAdd);
    if (_sourceAdd==0)
        _sourceAdd=_readUnits;
    scanf("%x",&_location);
    scanf("%d",&_length);
    if((_fileToDisplay=fopen(&_fileName,"r+"))==NULL){
        printf("Error: Unable to open file\n");
        return;
    }
    fseek(_fileToDisplay,_location,SEEK_SET);
    fwrite(_sourceAdd,1,_length,_fileToDisplay);
    fclose(_fileToDisplay);
    printf("%d Bytes were saved from location:0x%x, to file with offset:%d\n",_length,_sourceAdd,_location);
}

char * getNumToChar(int num){

    char * _toreturn=NULL;
    unsigned int i=0,_temp=getSizeOfVal(num);
    char sum=0;
    _toreturn=(char *)malloc(_temp);
    while (num>0){
        sum=num%16;
        num=num/16;
        sum=sum+((num%16)*16);
        printf("%d\n",sum);
        num=num/16;
        _toreturn[i]=sum;
        i++;
    }
    return _toreturn;
}
int getSizeOfVal(int num){
    unsigned int i=0;
    while(num>0){
        i++;
        num=num/256;
    }
    return i;
}

void fileModify(){
    /*what to do in case the length is a odd number? works only on even*/
    FILE * _fileToWorkOn=NULL;
    unsigned int _location, _val;
    char * _bufferVal=NULL;
    printf("Please enter <location> <val>\n");
    scanf("%x",&_location);
    scanf("%x",&_val);
    if(_inDebug)
        printf("Location:0x%x\tNew value:0x%x\n",_location,_val);
    if((_fileToWorkOn=fopen(&_fileName,"r+"))==NULL){
        printf("Error: Unable to open file\n");
        return;
    }
    _bufferVal = getNumToChar(_val);


    fseek(_fileToWorkOn,_location,SEEK_SET);
    printf("%d\n",getSizeOfVal(_val) );
    fwrite(_bufferVal,1,getSizeOfVal(_val),_fileToWorkOn);

    fflush(_fileToWorkOn);
    fclose(_fileToWorkOn);
    printf("Finished Modefing file\n");
}


/*********************UI****************************************/
void promptDebugPrint(){
    printf("Debug: Unit size:%d\tBuffer address:0x%x\tFile name:%s\n",_size,(unsigned int)_fileName,_fileName);
}

void displaymenu(){
        printf("Choose action:\n ");
        printf("0-Toggle Debug Mode\n ");
        printf("1-Set File Name\n ");
        printf("2-Set Unit Size\n ");
        printf("3-File Display\n ");
        printf("4-Load Into Memory\n ");
        printf("5-Save Into File\n ");
        printf("6-File Modify\n ");
        printf("7-Quit\n");
}

void menuPick(int _userChoice){
    char c;
    switch(_userChoice){
            case 0:
                toggleDebug();
                break;
            case 1:
                setFileName();
                break;
            case 2:
                setUnitSize();
                break;
            case 3:
                fileDisplay();
                break;
            case 4:
                loadToMem();
                break;
            case 5:
                saveToFile();
                break;
            case 6:
                fileModify();
                break;
            case 7:
                quitProg();
                break;
            default:
                while ((c = getchar()) != '\n' && c != EOF) { }
                break;
        }
}




int main(int argc, char *argv[]) {
    int _userChoice;
    while(1){
        if (_inDebug)
            promptDebugPrint();

        displaymenu();
        scanf("%d" , &_userChoice);
        menuPick(_userChoice);

    }
    return 0;
}
