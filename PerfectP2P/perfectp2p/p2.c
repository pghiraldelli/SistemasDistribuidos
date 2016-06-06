
/* Application to experiment the FairP2P module */

#include "perfectp2p.h"

#define SENDPORT 4000
#define RECVPORT 3000


// !!! Must be implemented to handle the indication event !!!
void fp2pDelivery (char *src, char *msg) {
  printf("Recebi: %s %s\n", src, msg);
  pp2pSend(src, msg);
}   

// main function
int main(int argc, char **argv) {
  pp2pInit(SENDPORT, RECVPORT);
  pthread_exit(NULL);
}
