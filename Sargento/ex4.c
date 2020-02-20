#include<stdio.h>
#include<stdlib.h>
int main(){
    int numbers[10000],i;
    

    for(i=0;i<10000;i++){
        numbers[i] = rand() % 10000;
        if (numbers[i]%2==0)
            printf("%d is a multiple of 2\n",numbers[i]);

        if (numbers[i]%3==0)
            printf("%d is a multiple of 3\n",numbers[i]);
        
        if (numbers[i]%5==0)
            printf("%d is a multiple of 5\n",numbers[i]);
        
        if (numbers[i]%7==0)
            printf("%d is a multiple of 7\n",numbers[i]);
    }


    return 0;
}

