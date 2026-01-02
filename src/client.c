#define MAXLINE 8192 /* Max text line length */
#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


int open_clientfd(char *ip, int port) {
  // The client's socket file descriptor.
  int clientfd;

  // The hostent struct is used to get the IP address of the server
  // using DNS.
  //
  // struct hostent {
  //   char *h_name;        // official domain name of host
  //   char **h_aliases;    // null-terminated array of domain names
  //   int  h_addrtype;     // host address type (AF_INET)
  //   int  h_length;       // length of an address, in bytes
  //   char **h_addr_list;  // null-terminated array of in_addr structs
  // };
  struct hostent *hp;

  // serveraddr is used to record the server information (IP address
  // and port number).
  //
  // struct sockaddr_in {
  //   short            sin_family;   // e.g. AF_INET
  //   unsigned short   sin_port;     // e.g. htons(3490)
  //   struct in_addr   sin_addr;     // see struct in_addr, below
  //   char             sin_zero[8];  // zero this if you want to
  // };
  struct sockaddr_in serveraddr;

  printf("Echo Client is creating a socket.\n");

  // First, we create the socket file descriptor with the given
  // protocol and protocol family.
  if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;


  // The socket API requires that you zero out the bytes!
  bzero((char *)&serveraddr, sizeof(serveraddr));

  // Record the protocol family we are using to connect.
  serveraddr.sin_family = AF_INET;

  serveraddr.sin_addr.s_addr = inet_addr(ip);
  // Convert the port from host byte order to network byte order and
  // store this in the server address structure.
  serveraddr.sin_port = htons(port);

  printf("Echo Client is trying to connect to (%s:%d).\n", ip, port);

  // Establish a connection with the server.
  if (connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    return -1;

  printf("Echo Client connected.\n");

  // Return the connected file descriptor.
  return clientfd;
}

int main(int argc, char* argv[]){
    //Remember to add null terminator so we can use string functions
    // The client socket file descriptor.
  int clientfd;
  // The port number.
  int port;
  // Variable to store the host/server domain name.
  char *host;
  char *identification;
  
    //We should check that there are three arguements
    //The three arguements are Identification, Port, and Host IP

    if (argc != 4) {
    fprintf(stderr, "usage: %s <indetification> <port> <host>\n", argv[0]);
    exit(0);
  }

  port = atoi(argv[2]);
  host = argv[3];
  identification = argv[1];

  clientfd = open_clientfd(host, port);

  if(clientfd < 0) {
    printf("Failed to connect to server");
    exit(0);
  }

  char recv_buffer[MAXLINE];
  char buffer[MAXLINE];

  snprintf(buffer, MAXLINE, "cs230 HELLO %s\n", identification);
    if(send(clientfd, buffer, strlen(buffer), 0)<=0){
      perror("send failed");
      //exit because error
      exit(1);
    }

    printf("SENT: %s",buffer);

  while(1){
    ssize_t bytes_received = recv(clientfd, recv_buffer, MAXLINE, 0);
    if(bytes_received<=0){
      perror("recv failed");
      //exit because error
      exit(1);
    }

    recv_buffer[bytes_received] = '\0';
    printf("RECEIVED: %s", recv_buffer);

    //Cehck if we got BYE
    if (strstr(recv_buffer, "BYE") != NULL) {
    printf("Flag received: %s\n", recv_buffer);
    break;  
    }

    //Check Operation and Number
    char operation[20];
    unsigned int number;

    //Gets the operation and number
    sscanf(recv_buffer, "cs230 STATUS %s %d", operation, &number);

    printf("Operation: %s\n", operation);
    printf("Number: %d\n", number);

    unsigned int result = 0;

    //Left Operations
    if(strcmp(operation,"LEFT")==0){
      int shift = number % 10;
      result = number << shift;
    }
    //Right Operation
    else if (strcmp(operation, "RIGHT") == 0) {
      int shift = number % 10;
      result = ((unsigned int)number) >> shift;
    }
    //Or Operation
    else if (strcmp(operation, "OR") == 0) {
      int least_sig = number & 0xF;
     unsigned int mask = least_sig * 0x11111111;
      result = number | mask;
    }
    //And operation
    else if (strcmp(operation, "AND") == 0) {
      int least_sig = number & 0xF;
      unsigned int mask = least_sig * 0x11111111;
      result = number & mask;
    }

    //Replaces string inside buffer with new result
    snprintf(buffer, MAXLINE, "cs230 %u\n", result);

  //Sends the result
  if(send(clientfd, buffer, strlen(buffer), 0) < 0){
      perror("send failed");
      exit(1);
    }
   printf("SENT: %s", buffer); 
  }

  close(clientfd);
  return 0;
}