#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main(int argc, char* argv[])
{
int i=0;
char* result;

result="";

while(i<argc){
    result = strcat(argv[i],result);    
    i++;
}

printf("%s\n",result);




}