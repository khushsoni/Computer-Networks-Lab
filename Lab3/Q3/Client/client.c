#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#define SERVER_PORT 3456
#define MAX_LINE 256

int main(int argc, char * argv[]){
  FILE *fp;
  struct hostent *hp;
  struct sockaddr_in sin;
  char *host;
  char buf[MAX_LINE];
  char sbuf[MAX_LINE];
  int s;
  int len;
  int port;
  if (argc==3) {
    host = argv[1];
    port = atoi(argv[2]);
  }
  else {
    fprintf(stderr, "usage: %s host port \n ", argv[0]);
    exit(1);
  }
  
  /* translate host name into peer's IP address */
  hp = gethostbyname(host);
  if (!hp) {
    fprintf(stderr, "%s: unknown host: %s\n", argv[0], host);
    exit(1);
  }
  else
    printf("Client's remote host: %s\n", argv[1]);
  
  /* build address data structure */
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
  sin.sin_port = htons(port);
  
  /* active open */
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("simplex-talk: socket");
    exit(1);
  }
  else
    printf("Client created socket.\n");

  if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
      perror("simplex-talk: connect");
      close(s);
      exit(1);
    }
  else
    printf("Client connected.\n");
  
  if (len = recv(s, buf, sizeof(buf), 0)) {
    buf[len] = '\0';
    printf("Client received: %s\n", buf);
  }

  /* send file requests to the server */
  while (1) {
  char fileNameBuf[MAX_LINE];
    printf("Enter filename: ");
    fgets(buf, sizeof(buf), stdin);
    // remove trailing newline from file name
    buf[strlen(buf) - 1] = '\0';
    for (int i = 0; i < strlen(buf); i++) {
    	
    fileNameBuf[i] = buf[i];
    }
//     strcpy(buf, fileNameBuf);
    send(s, buf, sizeof(buf), 0);
    //printf("File Name: %s, Len: %ld",buf,sizeof(buf));
    
    // receive "OK" or "File not found" message
    recv(s, buf, sizeof(buf), 0);
    printf("Received message: %s\n", buf);
    if (strcmp(buf, "File not found") == 0) {
      continue;
    }
    if (strcmp(buf, "Bye") == 0) {
      break;
    }
    // receive file data
    fp = fopen(fileNameBuf,"w");
    if(fp==NULL){
    	printf("Error creating a file");
    	return 1;
    }
    bzero(buf,sizeof(buf));
    int n=0;
    int total =0;
    
    while ((n = recv(s, buf, sizeof(buf), 0)) > 0) {
      printf("Received data: %s\n", buf);
      total += n;
      fwrite(buf, 1,n,fp);
      bzero(buf, sizeof(buf));
      if(n==0 || n!=MAX_LINE) break;
      // continue;
    }
    fclose(fp);
  }

  close(s);
}
