#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int dig_count(char * string);

int main(int argc,char ** argv){
    int counter;
    counter =dig_count(argv[1]);
    printf("The number of digits in the string is: %d\n",counter);
    return 0;
}

int dig_count(char * string){
    int counter=0;
    int i = 0;
    while (string[i]!='\0'){
        if(string[i]>='0' && string[i]<='9')
            counter++;
        i++;
    }
    return counter;
}