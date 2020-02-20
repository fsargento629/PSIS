#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char* concatenate(char* str1,char* str2);


int main(int argc, char* argv[])
{
   
int i=0;
char* result=argv[0]; 


for(i=1;i<argc;i=i+1){
    result=concatenate(result,argv[i]);


}


printf("%s\n",result);

return 0;
}

char* concatenate(char* str1,char* str2)
{
    
    char* resultado = malloc(100);
    int i=0,j=0;

    while(str1[i]!='\0'){
        resultado[i]=str1[i];
        i++;
    }

    while(str2[j]!='\0'){
        resultado[i]=str2[j];
        i++;
        j++;
    }
    resultado[i]='\0';
    
    
    return resultado;
    
}