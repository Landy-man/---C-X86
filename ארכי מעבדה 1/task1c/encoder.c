#include<stdio.h>
#include<string.h>


int main (int argc,char *argv[]){
    
    int _key;
    char * _cyphKey;
    int _sign=1;
    int _chaOfString;
    FILE *_fileStream = stdin;
    
    if (argc==1 || argc==3){
        _key=0;
    }
    else if (argc==2 || argc==4){
        _cyphKey = argv[1];
        if (*_cyphKey == '-'){
            _sign=-1;
            _key = *(_cyphKey+1) - 'A';
        }
        else if(*_cyphKey == '+')
            _key = *(_cyphKey+1) - 'A';
        else
            _key = *(_cyphKey) - 'A';
        _key = _key * _sign; 
    }
    else{
        printf("Error: unrecognized Variables. Exitting\n");
        return 1;
    }
    
    if(argc>2 && (strcmp(argv[argc-2],"-i")==0)){
        _fileStream = fopen(argv[argc-1],"r");
        if (_fileStream==NULL){
            printf("Error: Could not open file. Exitting\n");
            return 1;
        }
    }
    else
        printf("Enter text to encode:\n");
    _chaOfString = fgetc(_fileStream);
    while (_chaOfString!=EOF){
        if (_chaOfString>='A' && _chaOfString<='Z')
            fputc('a'+(_chaOfString+('a'-'A')+_key-'a'+26)%26,stdout);        
        else if (_chaOfString>='a' && _chaOfString<='z' )   
            fputc('a'+((_chaOfString+_key)-'a'+26)%26,stdout);
        else
            fputc(_chaOfString,stdout);
        _chaOfString = fgetc(_fileStream);
    }
    return fclose(_fileStream);
}