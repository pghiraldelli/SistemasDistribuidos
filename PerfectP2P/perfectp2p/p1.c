
/* Application to experiment the FairP2P module */

#include "perfectp2p.h"
#include "fairp2p.h"

#define SENDPORT 3000
#define RECVPORT 4000


// !!! Must be implemented to handle the indication event !!!
void pp2pDelivery (char *src, char *msg) {
  printf("Enviei: %s %s\n", src, msg);
}   

// main function
int main(int argc, char **argv) {
  char msg[BUFSIZE]; 
 
  if(argc < 2) 
     error("Digite <prog> <ip dest>");

  pp2pInit(SENDPORT, RECVPORT);

  for(int i=0;i<100; i++) {
     sprintf(msg, "Msg: %d", i);
     pp2pSend(argv[1], msg);
  }

  pthread_exit(NULL);
}
