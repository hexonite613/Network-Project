#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

//initalizing server structure
struct sockaddr_in server_addr;
//initalizing client structure
struct sockaddr_in client_addr;

void handle_request(int client_socket){
    //start from here
}


//main function(num is number of parameter from client and save is where we store parameters)
int main(int num, char *save[]){
    //check if user gave port number
    if(num!=2){
        //alert user
        printf("No port number");
        //cannot execute further so exit
        exit(EXIT_FAILURE);
    }

    //Bring user input into interger port number
    int port=atoi(save[1]);
    //check if user sent port number larger than 1024
    if(port<=1024){
        //alert user
        printf("use port number larger than 1024");
        exit(EXIT_FAILURE);
    }

    //initialize server socket
    int s_socket= socket(AF_INET,SOCK_STREAM,0);

    //if socket returns -1
    if(s_socket<0){
        //socket error
        perror("socket failed");
        //close server socket
        close(s_socket);
        //cannot execute further so exit
        exit(EXIT_FAILURE);
    }

    //set server domain(IPv4)
    server_addr.sin_family=AF_INET;
    //
    server_addr.sin_addr.s_addr=INADDR_ANY;
    //save and change port number into network bytes(16 bit num)
    server_addr.sin_port=htons(port);

    //bind socket and ip address, port number
    //if returns -1, return error
    if(bind(s_socket,(struct sockaddr *)&server_addr,sizeof(server_addr))<0){
        //bind error
        perror("bind failed");
        //close server socket
        close(s_socket);
        //cannot execute further so exit
        exit(EXIT_FAILURE);
    }

    //if listen returns -1, return error(try up to 5 queues)
    if(listen(s_socket,5)<0){
        //alert user
        perror("unable to listen");
        //close server socket
        close(s_socket);
        //cannot execute further so exit
        exit(EXIT_FAILURE);
    }

    //alert that server is on
    printf("server is listening on port %d\n",port);

    //try to get client request
    while(1){
        //initialize client address length
        socklen_t client_addr_len=sizeof(client_addr);
        //initialize client socket by using accept
        int client_socket=accept(s_socket,(struct sockaddr*)&client_addr,&client_addr_len);
        if (client_socket<0){
            //alert user
            perror("error on accept");
            //keep on trying
            continue;
        }
        //handle request method here
    }

    //close server socket
    close(s_socket);
    //end 
    return 0;
}