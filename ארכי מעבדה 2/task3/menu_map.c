#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFF_SIZE 100

/*Global variables*/
char* mapped_array=NULL;
char * carray = NULL;


/*Defineing a struck (a tapule (name,function)*/
struct fun_desc {
  char *name;
  char (*fun)(char);
};


/*declaring on various functions*/ 
char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}

char encrypt(char c) {
  if(c>='A' && c<='Z')
    return c-'A'+'a';
  else
    return c;
}

char decrypt(char c) {
  if(c>='a' && c<='z')
    return c-'a'+'A';
  else
    return c;
}

char xprt(char c){
    /*Taken from stackoverflow, how to print hex values in C*/
    printf("0x%X\n", c & 0xFF);
    return c;
}

char cprt(char c) {
  if(c>=0x20 && c<=0x7E)
    printf("%c\n" , c);
  else
    printf(".\n");
  return c;
}

char my_get(char c) {
  char _readChar = fgetc(stdin);
  if (_readChar=='\n')
      return '\0';
  return _readChar;
}


char quit(char c){
    if (mapped_array!=NULL)
        free(mapped_array);
    if (carray!=NULL)
        free(carray);
    exit(c);
}

/*the map function:
iterating on array and applying function f, on each element in array, returnning a new pointer to array with the result were for each element i in array, the result will be in mapped_array[i]*/ 
char* map(char *array, int array_length, char (*f) (char)){
  /*mapped_array = (char*)(malloc(array_length*sizeof(char)));*/
  int _loopCounter;
  char _clearChar;
  
  while((_clearChar = getchar()) != '\n' && _clearChar != EOF);  /*Clearing the stdin - Taken from Stackoverflow*/
  for (_loopCounter=0;_loopCounter<array_length;_loopCounter++){
      /*mapped_array[_loopCounter]=(*f)(*(array+_loopCounter));*/
      if((carray[_loopCounter]=(*f)(*(array+_loopCounter)))==0)
          break;
  }
  return mapped_array;
}
 
int main(int argc, char **argv){
    /*Variables*/
    struct fun_desc _funcArray[] ={ {"Censor", &censor},{"Encrypt", &encrypt},{"Decrypt", &decrypt},{"Print hex", &xprt},{"Print string", &cprt},{"Get string", &my_get},{"Quit", &quit}, {0, 0}  };
    int _loopCounter;
    int _userOption;
    
    /*Code*/
    carray = (char*)calloc(BUFF_SIZE, sizeof(char));
    do{
    printf("Please choose a function:\n");
    for(_loopCounter=0;_funcArray[_loopCounter].name!=NULL;_loopCounter++){
        printf("%d) %s\n",_loopCounter,_funcArray[_loopCounter].name);
    }
    printf("Option: ");
    _userOption=fgetc(stdin)- '0';
    if(_userOption<_loopCounter && _userOption>0){
        printf("Within bounds\n");
        map(carray,BUFF_SIZE,_funcArray[_userOption].fun);
        printf("Done.\n \n");
        
    }
    else{
        printf("Not within bounds\n");
        printf("\n \n%d\n\n", _userOption);
        quit(0);
    }
    } while(1);
    return 0;
}