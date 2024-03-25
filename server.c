//Includes libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

//initalize buffer_size to reduce memory usage
#define BUFFER_SIZE 1000
#define SERVER_DIRE "/home/hexonite/network project/"

//initalizing server structure
struct sockaddr_in server_addr;
//initalizing client structure
struct sockaddr_in client_addr;

//method to handle request
void handle_request(int client_socket){
    //initialize buffer
    char buffer[BUFFER_SIZE];
    
    //initialize size of buffer received
    ssize_t leng_received;
    //get received length
    leng_received=recv(client_socket,buffer,BUFFER_SIZE,0);
    
    //if received -1 for an error
    if(leng_received<0){
        //alert user
        perror("Error receiving from client");
        //close socket
        close(client_socket);
        return;
    }

    //print request has been received
    printf("Received request:\n%s",buffer);


    //parse the request
    char method[10], path[256];
    sscanf(buffer, "%s %s", method, path);

    //open the requested file
    FILE *file = fopen(&path[1], "r");

    //if file was not found
    if (file == NULL) {
        // send 404 code(file not found)
        char not_found[] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
        //alert user file was not found
        send(client_socket, not_found, strlen(not_found), 0);
    
    }
    //to return found file 
    else {
        // send code 200(successfully found)
        char response_header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        //send success code in header file to client
        send(client_socket, response_header, strlen(response_header), 0);
        //send file to client until end of the file
        while ((leng_received = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
            //send file to client
            send(client_socket, buffer, leng_received, 0);
        }
        //close opened file
        fclose(file);
    }
    //close client socket
    close(client_socket);
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
        //cannot execute further so exit
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
        //call handle_request method to parse client request
        handle_request(client_socket);
    }

    //close server socket
    close(s_socket);
    //end 
    return 0;
}
