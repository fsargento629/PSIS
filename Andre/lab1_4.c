#include <stdio.h>
#include <stdlib.h>

#define CYCLES 10000

void check(int number, int div)
{
    if(number % div == 0)
        printf("%d is a multiple of %d \n", number, div);
}

void main(){
    int i;
    int random_numb;

    for(i=0; i<CYCLES; i++)
    {
        random_numb = rand();
        check(random_numb, 2);
        check(random_numb, 3);
        check(random_numb, 5);
        check(random_numb, 7);
    }   

    exit(0);
}
