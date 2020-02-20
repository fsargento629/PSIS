#include <stdio.h>
#include <stdlib.h>

#define CYCLES 10000

int main(){
    int i;
    int random_numb;

    for(i=0; i<CYCLES; i++)
    {
        random_numb = rand();
        if(random_numb % 2 == 0)
            printf("%d ", random_numb);
            else if(random_numb % 3 == 0)
                printf("%d ", random_numb);
                else if(random_numb % 5 == 0)
                    printf("%d ", random_numb);
                    else if(random_numb % 7 == 0)
                        printf("%d ", random_numb);
    }   

}