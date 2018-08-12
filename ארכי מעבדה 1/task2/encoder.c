#include<stdio.h>
#include<string.h>




int main (int argc,char *argv[]){

	int i;
	FILE * _fileStreamInput=stdin;
        FILE * _fileStreamOutput=stdout;
	int _key=0;
	char * _cyphKey;
	int _chaOfString;
	int _sign=1;
        
        
	/*

	beginning finding all variables and initilazing relevent variables
	and flags


	*/
	for (i=1;i<argc;i++){
		/*checking for "-i FileName" struct*/  
		if (strcmp(argv[i],"-i")==0){
			_fileStreamInput=fopen(argv[i+1],"r");
			if (_fileStreamInput==NULL){
				printf("Error: Cannot open the input file. Exitting\n");
				return 1;
			}
			i++; /*jumping i over the file name in order not to mix with the key*/
		}
		/*Checking whether to print to screen or file*/
		else if(strcmp(argv[i],"-o")==0){
                        _fileStreamOutput=fopen(argv[i+1],"w");
                        if (_fileStreamOutput==NULL){
				printf("Error: Cannot open the output file. Exitting\n");
				return 1;
			}
			i++; /*jumping the i over the output file name */
                }
		/*last parameter is key if it is not on of the above*/
		else {
                        _cyphKey = argv[i];
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
	}	
	/*getting the first char from the relevent stream (old school Do-While)*/
	if (_fileStreamInput==stdin){
                printf("Enter text to encode:\n");
		_chaOfString = fgetc(stdin);
        }
	else
		_chaOfString = fgetc(_fileStreamInput);
	while (_chaOfString!=EOF){
		if (_chaOfString>='A' && _chaOfString<='Z'){
				fputc('a'+(_chaOfString+('a'-'A')+_key-'a'+26)%26,_fileStreamOutput);
		}
		else if (_chaOfString>='a' && _chaOfString<='z' ){
				fputc('a'+((_chaOfString+_key)-'a'+26)%26,_fileStreamOutput);
		}
		else{
				fputc(_chaOfString,_fileStreamOutput);
		}
			_chaOfString = fgetc(_fileStreamInput);
	}
        fclose(_fileStreamOutput);
        fclose(_fileStreamInput);
	return 0;
}