#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>
#include "common.h"
#include <pthread.h>

// handles some signals, like SIGPIPE
/*void signal_kill_handler( int signum){
    printf("Shutting down due to Ctrl-C signal\n");
    exit(0);
}*/
// used to process a disconnect

