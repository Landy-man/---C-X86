#include <stdio.h>

extern void calc_div(int x, int k);

int check (int x, int k){
    return ((x>0)&&(k>-1)&&(k<=31));
}

int main(int argc, char** argv)
{
    int x=0,k=0;
    scanf("%d", &x);
    scanf("%d", &k);
    calc_div(x,k);
    
    return 0;
    

    
}
