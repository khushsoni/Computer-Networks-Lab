/* CSE 342 Computer Networks, Fall 2023
   Lab 3, Client
   Section 1
   AU2040239- Khush Soni
*/

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

int main(int argc, char *argv[])
{
    FILE *fp;
    struct hostent *hp;
    struct sockaddr_in sin;
    char *host;
    char bf[MAX_LINE];
    int s;
    int len;
    int n;
    if (argc == 2)
    {
        host = argv[1];
    }
    else
    {
        fprintf(stderr, "usage: %s host\n", argv[0]);
        exit(1);
    }

    /* translating the host name into peer ones IP address */
    hp = gethostbyname(host);
    if (!hp)
    {
        fprintf(stderr, "%s: unknown host: %s\n", argv[0], host);
        exit(1);
    }
    else
        printf("The remote host of client ones: %s\n", argv[1]);

    /* creating add. of datastructure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    sin.sin_port = htons(SERVER_PORT);

    /* openly activating */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("simplex-talk: socket");
        exit(1);
    }
    else
        printf("The socket is created by clients.\n");

 if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
      perror("simplex-talk: connect");
      close(s);
      exit(1);
    }
  else
    printf("CLinets is connects.\n");
  
  if (len = recv(s, bf, sizeof(bf), 0)) {
    bf[len] = '\0';
    printf("Client received: %s\n", bf);
  }
  while (1) {
  char fileBf[MAX_LINE];
    printf("Enter filename: ");
    fgets(bf, sizeof(bf), stdin);
    // deleting tail line from name of the file 
    bf[strlen(bf) - 1] = '\0';
    
    for (int i = 0; i < strlen(bf); i++) {
    	
    fileBf[i] = bf[i];
    }
    send(s, bf, sizeof(bf), 0);

    recv(s, bf, sizeof(bf), 0);
    printf("The message received is: %s\n", bf);
    if (strcmp(bf, "File could not be found") == 0) {
      continue;
    }
    if (strcmp(bf, "Bye") == 0) {
      break;
    }
    // receive file data
    fp = fopen(fileBf,"w");
    if(fp==NULL){
    	printf("File is not created.");
    	return 1;
    }
    bzero(bf,sizeof(bf));
    int n=0;
    int total =0;
    
    while ((n = recv(s, bf, sizeof(bf), 0)) > 0) {
      printf("The file consisting elemnts : %s\n", bf);
      total += n;
      fwrite(bf, 1,n,fp);
      bzero(bf, sizeof(bf));
      if(n==0 || n!=MAX_LINE) break;
    }
  fclose(fp);
  }
