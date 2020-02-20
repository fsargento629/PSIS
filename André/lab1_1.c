#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void conc(char *one, char *two)
{
    while(*one != '\0') /* Reaches the end of the first string*/
        one++;

    while(*two != '\0') /* Adds each element of the second string to the first string */
    {
        *one = *two;
        one++;
        two++;
    }

    *one = '\0'; /* Finishes the final string */
}


int main(int argc, char * argv[]){
    /* 
    argv - vetor de endereços do primeiro elemento de cada string
    argc - numero de elementos do vetor 
    */

    int i;
    int size;
    char *result_str;

    for(i=0; i<argc; i++)
    {
        size += strlen(argv[i]);
    }

    result_str = (char*)calloc(size+1, sizeof(char)); /* Allocates memory space (size+1 to add the last \0) */

    for(i=0; i<argc; i++)
    {
        conc(result_str, argv[i]);
    }

    printf("%s \n", result_str);
    free(result_str);
    exit(0);
}

