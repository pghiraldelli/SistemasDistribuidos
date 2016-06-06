/* 
 * Algorithm FairP2P 
 */

#include "perfectp2p.h"

const char vec_id[1000];

int recebeuMsg(char* id){
  int i;

  for(i = 0; i < 100; i++){
    if(strcmp(vec_id[i], id) == 0){
      return 1;
    }
  }

  return 0;
}

// send vai chamar várias vezes o fp2psend
void pp2pSend (char *dest, char *msg) { 
  int i;
  static int id = 0;
  id++;

  char str[1000];
  sprintf(str, "%d|%s\n", id, msg);

  for (i = 0; i < 10; i++){
    fp2pSend(dest, str);
    usleep(100000);
  }
}

// verificar se a mensagem já foi recebida, se não foi, vai chamar o delivery do fairp2p
int pdelivery (char *src, char *msg) {
  //quebrar msg pra pegar id
  char* id_src = strtok(msg, "|");
  char* msgEnviar = strtok(NULL, "\n");

  //sprintf
  strcat(id_src, src);

  if(!recebeuMsg(id_src)){
    delivery(src, msgEnviar);
    //amazenar id na posição certa do vetor
  }
}

// FairP2P init event
void pp2pInit(int pp2psendPort, int pp2precvPort) {
  fp2pInit(pp2psendPort, pp2precvPort);
}
