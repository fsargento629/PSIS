#include "lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

/* include the correct .h file */


int main(){
	int a;
	char line[100];
	char library_name[100];
	void *handle;
	char *error;
	void (*func1)(void), (*func2)(void);

	printf("What version of the functions you whant to use?\n");
	printf("\t1 - Normal    (lib1)\n");
	printf("\t2 - Optimized (lib2)\n");
	fgets(line, 100, stdin);
	sscanf(line,"%d", &a);
	if (a == 1){
		strcpy(library_name, "./lib1.so");
		printf("running the normal versions from %s \n", library_name);
	}else{
		if(a== 2){
			strcpy(library_name, "./lib2.so");
			printf("running the normal versions %s \n", library_name);
		}else{
			printf("Not running anything\n");
			exit(-1);
		}
	}
	/* load library from name library_name */

	handle = dlopen(library_name, RTLD_LAZY);
	if (!handle) {
            fputs (dlerror(), stderr);
            exit(1);
        }
	/* declare pointers to functions */

	/*load func_1 from loaded library */

	func1 = dlsym(handle, "func_1");
	 if ((error = dlerror()) != NULL)  {
            fputs(error, stderr);
            exit(1);
        }
	
	/*load func_2 from loaded library */

	func2 = dlsym(handle, "func_2");
	 if ((error = dlerror()) != NULL)  {
            fputs(error, stderr);
            exit(1);
        }

	/* call func_1 from whichever library was loaded */
	func1();
	/* call func_2 from whichever library was loaded */
	func2();
	exit(0);
}
