/* CSE342 Computer Networks, Fall 2023
   Lab 4, server 
   AU2040239- Khush Soni
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// NEW LIBRARY ADDED---
#include<time.h>

#define SERVER_PORT 5432
#define CHUNK_SIZE 4096


int main(int argc, char * argv[]){
  struct sockaddr_in sin;
  struct sockaddr_storage client_addr;
  char clientIP[INET_ADDRSTRLEN]; /* For IPv4 addresses */
  socklen_t client_addr_len;
  char bf[CHUNK_SIZE];
  int ln;
  int s;
  char *host;
  struct hostent *hp;
  //ADDED FILE POINTER
  FILE *fl;

  /* Declarations for file(s) to be sent 
     ...
  */
  fl = fopen("video.mp4", "rb");
  if(fl == NULL){
  	fprintf(stderr,"File could not found.");
  	exit(1);
  }
  
  /* For inserting delays, use nanosleep()
     struct timespec ... */   
     struct timespec t;
     t.tv_sec = 0;
     t.tv_nsec = 20000000L;
     
   
  /* Create a socket */
  if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("server: socket");
    exit(1);
  }
  
  /* build address data structure and bind to all local addresses*/
  memset((char *)&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  
  /* If socket IP address specified, bind to it. */
  if(argc == 2) {
    host = argv[1];
    hp = gethostbyname(host);
    if (!hp) {
      fprintf(stderr, "server: unknown host %s\n", host);
      exit(1);
    }
    memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
  }
  /* Else bind to 0.0.0.0 */
  else
    sin.sin_addr.s_addr = INADDR_ANY;
  
  sin.sin_port = htons(SERVER_PORT);
  
  if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
    perror("server: bind");
    exit(1);
  }
  else{
    /* Add code to parse IPv6 addresses */
    inet_ntop(AF_INET, &(sin.sin_addr), clientIP, INET_ADDRSTRLEN);
    printf("Server is listening at address %s:%d\n", clientIP, SERVER_PORT);
  }
  
  printf("Client needs to send \"GET\" to receive the file %s\n", argv[1]);
    
  
  client_addr_len = sizeof(client_addr);
  
  /* Receive messages from clients*/
  while(ln = recvfrom(s, bf, sizeof(bf), 0,
           (struct sockaddr *)&client_addr, &client_addr_len)){
    
    inet_ntop(client_addr.ss_family,
        &(((struct sockaddr_in *)&client_addr)->sin_addr),
        clientIP, INET_ADDRSTRLEN);
    
    printf("Server got message from %s: %s [%d bytes]\n", clientIP, bf, ln);
	
    /* Send to client */
    /* Add code to send file if the incoming message is GET */

    // Check if the request is a GET request
if (strncmp(bf, "GET", 3) == 0) {
    // Clear the buffer
    bzero(bf, sizeof(bf));

    // Read the file and send its contents in chunks
    for (;;) {
    // Read a chunk of data from the file
    size_t ln = fread(bf, 1, CHUNK_SIZE, fl);
    // If there is no more data to read, break the loop
    	if (ln == 0) {
        break;
    	}
    	// Send the data to the client
    	sendto(s, bf, ln, 0, (struct sockaddr *)&client_addr, client_addr_len);
    	// Sleep for a short time to avoid flooding the network
    	nanosleep(&t, NULL);

	} 

    // Close the file
    fclose(fl);
    // bzero(bf, sizeof(bf));
    }

    
    memset(bf, 0, sizeof(bf));
  }
  
  /* Send BYE to signal termination */
  strcpy(bf, "BYE");
  sendto(s, bf, sizeof(bf), 0, 
   (struct sockaddr*)&client_addr, client_addr_len);

}

