#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include<stdio.h>
#include"server.h"


board_data_struct read_board_data(char* file_name){
    int size[2];
    char buff[100];
    FILE* fp=fopen(file_name,'r');
    board_data_struct ret;
    if(fp==NULL){
        printf("%s not found\n",file_name);
        exit(-1);
    }

    fgets(buff,sizeof(buff),fp);
    sscanf(buff,"%d %d",size[0],size[1]);

    ret.board_size[0]=size[0];
    ret.board_size[1]=size[1];


    return ret;

}




void init_server(){
    struct sockaddr_in server_local_addr;
    server_socket=socket(AF_INET,SOCK_STREAM,0);
    if(server_socket==-1){
        perror("socket:");
        exit(-1);
    }
    server_local_addr.sin_family=AF_INET;
    server_local_addr.sin_addr.s_addr=INADDR_ANY;
    server_local_addr.sin_port=htons(3000);
    int err=bind(server_socket,(struct sockaddr*)&server_local_addr,
                sizeof(server_local_addr));
    if(err==-1){
        perror("bind:");
        exit(-1);
    }

    // should we do the listen here? idont think so

}

void* accept_thread(void*arg){
    struct sockaddr_in client_addr;
    socklen_t size_addr = sizeof(client_addr);
    pthread_t client_thread_ids[MAXPLAYERS];
    int client_fd,i=0;
    if(listen(server_socket,MAXPLAYERS)==-1){
        perror("listen");
        exit(-1);
    }
    printf("Waiting for connections\n");
    i=0;
    while(1){
        client_fd=accept(server_socket,&client_addr,&size_addr);
        if(client_fd==-1){
            perror("accept:");
            exit(-1);
        }
        printf("Accepted connection\n");
        pthread_create(&client_thread_ids[i],NULL,client_thread,NULL);
        i++;
    }

}


// Thread that receives updates from each client and triggers an event (?)
void* client_thread(void* client_args){
    client_thread_args args = *(client_thread_args*)client_args;
    int client_fd=args.fd;
    int err_rcv;
    C2S_message msg;
    while((err_rcv = recv(client_fd,&msg,sizeof(msg),0))>0 ){
        printf("recived %d bytes\n",err_rcv);
        // handle message from client

    }


}
