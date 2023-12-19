#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_PORT 5432
#define MAX_PENDING 5
#define MAX_LINE 256
#define FILE_SIZE 4096

int main(int argc, char * argv[]){

  struct sockaddr_in sin;
  char buf[MAX_LINE];
  char rbuf[MAX_LINE];
  socklen_t len;
  int len2;
  int s, new_s;
  char str[INET_ADDRSTRLEN];


  /* build address data structure */
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = inet_addr("10.0.2.15");
  sin.sin_port = htons(SERVER_PORT);
  
  /* setup passive open */
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("simplex-talk: socket");
    exit(1);
  }
 
  inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);
  printf("Server is using address %s and port %d.\n", str, SERVER_PORT);

  if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
    perror("simplex-talk: bind");
    exit(1);
  }
  else
    printf("Server bind done.\n");

  listen(s, MAX_PENDING);
  
  /* wait for connection, then receive and print text */
  while(1) {
  //New Connection being setup
    if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
      perror("simplex-talk: accept");
      exit(1);
    }
    //connection done
    printf("Server Listening.\n");
    send(new_s,"Hello\n",6,0);
    //receiving file names
    while(1) {
      char filename[MAX_LINE];
      if (!(len=recv(new_s, filename, sizeof(filename), 0))) break;
      printf("File Name: '%s' \n",filename);
      filename[sizeof(filename)-1] = '\0';
      if(strncmp(filename, "Bye",3)==0){
      	printf("Client Disconnected \n");
      	break;
      }
      printf("File Requested: %s \n",filename);
      
      FILE *fp;
      fp = fopen(filename, "rb");
      if (fp == NULL) {
        send(new_s, "File not found", 14, 0);
        printf("Server: Sent 'File not found'\n");
      } else {
        char buffer[FILE_SIZE];
        send(new_s, "OK        ", 10, 0);
        printf("Server: Sent 'OK'\n");
        while (!feof(fp)) {
          int n = fread(buffer, 1, FILE_SIZE, fp);
          send(new_s, buffer, n, 0);
        }
        fclose(fp);
      }
    }
	  
    close(new_s);
  }
  }
