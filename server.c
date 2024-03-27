//Includes libraries
//for standard output
#include <stdio.h>
//changing format and control memory
#include <stdlib.h>
//for controlling strings
#include <string.h>
//for controlling files
#include <unistd.h>
//for using sockets
#include <sys/socket.h>
//for socket struct
#include <netinet/in.h>
//for using header files
#include <fcntl.h>
//for getting file name/directory
#include <libgen.h>

//initalize buffer_size
#define BUFFER_SIZE 1000
//need to change server directory into relative directory
#define SERVER_DIRE "/home/hexonite/network project/"

//initalizing server structure
struct sockaddr_in server_addr;
//initalizing client structure
struct sockaddr_in client_addr;



//made to get file extension
const char* get_file_extension(const char* filename){
    //get end of '.'
    const char* dot=strrchr(filename,'.');
    //if no file extension were found
    if(!dot){
        //just return plain text
        return "text/plain";
    }
    //for html
    if(strcmp(dot,".html")==0){
        //return html file format
        return "text/html";
    }
    //for gif
    if(strcmp(dot,".gif")==0){
        //return gif file format
        return "image/gif";  
    }
    //for jpeg
    if(strcmp(dot,".jpeg")==0){
        //return jpeg file format
        return "image/jpeg";  
    }
    //for mp3
    if(strcmp(dot,".mp3")==0){
        //return mp3 file format
        return "application/mp3";
    }
    //for pdf
    if(strcmp(dot,".pdf")==0){
        //return pdf file format
        return "application/pdf";  
    }
}

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
        //return to end
        return;
    }

    //print request has been received
    printf("Received request:\n%s",buffer);

    //initialize path and method
    char path[256];
    //parse the request(initial part represent method but since it will be not used, just skipped)
    sscanf(buffer, "*%s %s", path);

    //getting file extension
    char* file_path= basename(path);

    //open the requested file(read-only)
    FILE *file = fopen(&path[1], "r");

    //if file was not found
    if (file == NULL) {
        //send 404 code(file not found)
        char not_found[] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
        //alert user file was not found
        send(client_socket, not_found, strlen(not_found), 0);
    
    }
    //to return found file 
    else {
        //bring found file extension
        const char* file_ext=get_file_extension(path);
        //initialize header
        char response_header[256];
        //parse response code and file extension to header
        sprintf(response_header, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n",file_ext);
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
    //bind server_addr to listen from network interface
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
        //if client socket returns error
        if (client_socket<0){
            //alert user
            perror("error on accept");
            //keep on trying
            continue;
        }
        //call handle_request method to apply client request
        handle_request(client_socket);
    }

    //close server socket
    close(s_socket);
    //end 
    return 0;
}
