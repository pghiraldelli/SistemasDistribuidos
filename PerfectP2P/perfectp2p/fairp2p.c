/* 
 * Algorithm FairP2P 
 */

#include "fairp2p.h"

//ports to receive and send messages
int sendPort = 3000;
int recvPort = 3000;

// wrapper for perror
void error(char *msg) {
  perror(msg);
  exit(1);
}

// Request FairP2P event: send
int fp2pSend (char *dest, char *msg) { 
  int sock; /* socket */
  socklen_t serverlen; /* byte size of server's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct hostent *server;  /* server host info */

  /* socket: create the socket */
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) 
      error("ERROR opening socket");

  /* gethostbyname: get the server's DNS entry */
  server = gethostbyname(dest);
  if (server == NULL) 
      error("ERROR dest invalid");

  /* build the server's Internet address */
  memset((void *) &serveraddr, 0, sizeof(serveraddr)); 
  serveraddr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
        (char *)&serveraddr.sin_addr.s_addr, server->h_length);
  serveraddr.sin_port = htons(sendPort);

  /* send the message to the server */
  serverlen = sizeof(serveraddr);
  return sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) &serveraddr, serverlen);
}

// receive a message
int delivery (char *src, char *msg) {
  int sock; /* socket */
  socklen_t clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */
  char *hostaddrp; /* dotted decimal host addr string */

  /* socket: create the socket */
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
   error("ERROR opening socket");

  /* Eliminates "ERROR on binding: Address already in use" error. */
  optval = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
           (const void *)&optval , sizeof(int));

  /* build the server's Internet address */
  memset((void *) &serveraddr, 0, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short) recvPort);

  /* bind: associate the parent socket with a port */
  if (bind(sock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
     error("ERROR on binding");

  /* recvfrom: receive a UDP datagram from a client */
  clientlen = sizeof(clientaddr);
  memset(msg, 0, BUFSIZE);
  n = recvfrom(sock, msg, BUFSIZE, 0, (struct sockaddr *) &clientaddr, &clientlen);
  if (n < 0) return n;

  /* gethostbyaddr: determine who sent the datagram */
  hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
                        sizeof(clientaddr.sin_addr.s_addr), AF_INET);
  memset(src, 0, BUFSIZE);
  if (hostp == NULL)
   error("ERROR on gethostbyaddr");
  hostaddrp = inet_ntoa(clientaddr.sin_addr);
  if (hostaddrp == NULL)
   error("ERROR on inet_ntoa\n");

  strcpy(src, hostaddrp);

  return n;
}

// Indication FairP2P event: delivery
// Internal thread to wait for messages from other nodes
void * mythread(void * arg) {
  char msg[BUFSIZE]; /* message buf */
  char src[BUFSIZE]; /* source node */
  printf("Thread to receive messages is run...\n");
  while(1) {
    delivery(src, msg);
    fp2pDelivery(src, msg);
  }
  pthread_exit(NULL);
}

// FairP2P init event
void fp2pInit(int fp2psendPort, int fp2precvPort) {
  sendPort = fp2psendPort; //port to send messages to dst nodes
  recvPort = fp2precvPort; //port to receive messages from src nodes

  pthread_t tid;
  pthread_create(&tid, NULL, mythread, NULL);
}

