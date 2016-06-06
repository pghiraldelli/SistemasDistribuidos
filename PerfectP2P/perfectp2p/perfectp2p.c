/* 
 * Algorithm FairP2P 
 */

#include "perfectp2p.h"

const char vec_id[100][1000];

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

//a camada de cima vai implementar o fairdelivery 
void fp2pDelivery (char *src, char *msg) {
  //quebrar msg pra pegar id
  char* tmp = strtok(msg, "|");
  char id_src[1000];
  strcpy(id_src, tmp);
  
  char msgEnviar[1000];  
  char *tmp2 = strtok(NULL, "\n");
  strcpy(msgEnviar, tmp2);
  
  static int indice_vec = 0;

  strcat(id_src, "|");
  strcat(id_src, src);
  
  if(!recebeuMsg(id_src)){
    pp2pDelivery(src, msgEnviar);
    sprintf(vec_id[indice_vec++], "%s", id_src);
  }
  
}

// FairP2P init event
void pp2pInit(int pp2psendPort, int pp2precvPort) {
  fp2pInit(pp2psendPort, pp2precvPort);
}
