/* CSE342 Computer Networks, Fall 2023
   Lab 4, client 
   AU2040239- Khush Soni
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 5432
#define CHUNK_SIZE 8192


int main(int argc, char * argv[]){
  
  FILE *fl;
  struct hostent *hp;
  struct sockaddr_in sin;
  char *host;
  char bf[CHUNK_SIZE];
  int s;
  int ln;

  if ((argc==2)||(argc == 3)) {
    host = argv[1];
  }
  else {
    fprintf(stderr, "usage: client serverIP [download_name(optional)]\n");
    exit(1);
  }

  if(argc == 3) {
    fl = fopen(argv[2], "w");
    if (fl == NULL) {
      fprintf(stderr, "Error opening output file\n");
      exit(1);
    }
  }

  /* translate host name into peer's IP address */
  hp = gethostbyname(host);
  if (!hp) {
    fprintf(stderr, "client: unknown host: %s\n", host);
    exit(1);
  }
  else
    printf("Host %s found!\n", argv[1]);

  /* build address data structure */
  memset((char *)&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
  sin.sin_port = htons(SERVER_PORT);
  

  /* create socket */
  if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("client: socket");
    exit(1);
  }
  
  printf("Client will get data from to %s:%d.\n", argv[1], SERVER_PORT);
  printf("To play the music, pipe the downlaod file to a player, e.g., ALSA, SOX, VLC: cat recvd_file.wav | vlc -\n"); 
  
  
  

  /* send message to server */  
  fgets(bf, sizeof(bf), stdin);
  // strcpy(bf, "GET");
  bf[CHUNK_SIZE-1] = '\0';
  ln = strlen(bf) + 1;
  if (sendto(s, bf, ln, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) { 
    perror("Client: sendto()"); 
    return 0; 
  }
    
     
	bzero(bf, sizeof(bf));
	fl = fopen("video.mp4", "wb");
	for (;;) {
    	int i = recvfrom(s, bf, sizeof(bf), 0, NULL, NULL);
    	if (i <= 0) {
        	break;
    		}
    	fwrite(bf, i, 1, fl);
    	bzero(bf, sizeof(bf));
	}	
	fclose(fl);
	close(s);
   
  return 0;
  
}
