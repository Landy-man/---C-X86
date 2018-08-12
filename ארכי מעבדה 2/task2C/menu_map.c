#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFF_SIZE 100
 
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
    exit(c);
}

 
char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  int _loopCounter;
  for (_loopCounter=0;_loopCounter<array_length;_loopCounter++){
      if((mapped_array[_loopCounter]=(*f)(*(array+_loopCounter))) == 0)
          break;
  }
  return mapped_array;
}
 
int main(int argc, char **argv){
    char arr1[BUFF_SIZE];
    char* arr2 = map(arr1, BUFF_SIZE, my_get);
    char* arr3 = map(arr2, BUFF_SIZE, encrypt);
    char* arr4 = map(arr3, BUFF_SIZE, xprt);
    char* arr5 = map(arr4, BUFF_SIZE, decrypt);
    char* arr6 = map(arr5, BUFF_SIZE, cprt);
    free(arr2);
    free(arr3);
    free(arr4);
    free(arr5);
    free(arr6);
    quit('0');
    return 0;
}