// A multithread echo server 

#include "mysocket.h"  
#include <pthread.h>

#define BUFSIZE 100
#define NTHREADS 100

/* Structure of arguments to pass to client thread */
struct TArgs {
  TSocket cliSock;   /* socket descriptor for client */
};

int qtdClientes = 0;

void realizaSoma(char *str){
	int num1 = atoi(strtok(NULL, " "));
   	char* operador = strtok(NULL, " ");
	int num2 = atoi(strtok(NULL, " "));
	
	if(strcmp(operador, "+") == 0){
		sprintf(str, "resultado: %d\n", num1+num2);
	}else{
		sprintf(str, "erro ao realizar soma!\n");
	}
}

void realizaSubtracao(char *str){
	int num1 = atoi(strtok(NULL, " "));
   	char* operador = strtok(NULL, " ");
	int num2 = atoi(strtok(NULL, " "));
	
	if(strcmp(operador, "-") == 0){
		sprintf(str, "resultado: %d\n", num1-num2);
	}else{
		sprintf(str, "erro ao realizar subtração!\n");
	}
}

void ecoaString(char *str){
	char* msg = strtok(NULL, " ");
	sprintf(str, "%s", msg);
}

void numClientes(char *str){
	sprintf(str, "Clientes ativos: %d\n", qtdClientes);
}

void chamaOperacao(char *str){
	char* operacao = strtok(str, " ");

	if (strcmp(operacao,"soma")==0) realizaSoma(str);
	else if (strcmp(operacao,"subtrai")==0) realizaSubtracao(str);
	else if (strcmp(operacao,"ecoa")==0) ecoaString(str);
	else if (strcmp(operacao,"numClientes")==0) numClientes(str);
}

/* Handle client request */
void * HandleRequest(void *args) {
  char str[BUFSIZE];
  TSocket cliSock;

  /* Extract socket file descriptor from argument */
  cliSock = ((struct TArgs *) args) -> cliSock;
  free(args);  /* deallocate memory for argument */

  for(;;) {
    /* Receive the request */
    if (ReadLine(cliSock, str, BUFSIZE-1) < 0) { 
	ExitWithError("ReadLine() failed"); 
    } else {
	chamaOperacao(str);
    }
    if (strncmp(str, "quit", 4) == 0) break; 
 
    /* Send the response */
    if (WriteN(cliSock, str, strlen(str)) <= 0)  
      { ExitWithError("WriteN() failed"); }  
  }
  close(cliSock);
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  TSocket srvSock, cliSock;        /* server and client sockets */
  struct TArgs *args;              /* argument structure for thread */
  pthread_t threads[NTHREADS];
  int tid = 0;
  fd_set set;  /* file description set */
  int ret, i;
  char str[BUFSIZE];

  if (argc == 1) { ExitWithError("Usage: server <local port>"); }

  /* Create a passive-mode listener endpoint */  
  srvSock = CreateServer(atoi(argv[1]));

  printf("Server read!\n");
  /* Run forever */
  for (;;) { 
    /* Initialize the file descriptor set */
    FD_ZERO(&set);
    /* Include stdin into the file descriptor set */
    FD_SET(STDIN_FILENO, &set);
    /* Include srvSock into the file descriptor set */
    FD_SET(srvSock, &set);

    /* Select returns 1 if input available, -1 if error */
    ret = select (FD_SETSIZE, &set, NULL, NULL, NULL);
    if (ret<0) {
       WriteError("select() failed"); 
       break;
    }

    /* Read from stdin */
    if (FD_ISSET(STDIN_FILENO, &set)) {
      scanf("%99[^\n]%*c", str);
      if (strncmp(str, "TERMINAR", 3) == 0) break;
    }

    /* Read from srvSock */
    if (FD_ISSET(srvSock, &set)) {
      if (tid == NTHREADS) { 
        WriteError("number of threads is over"); 
        break; 
      }
      
      /* Spawn off separate thread for each client */
      cliSock = AcceptConnection(srvSock);

      /* Create separate memory for client argument */
      if ((args = (struct TArgs *) malloc(sizeof(struct TArgs))) == NULL) { 
        WriteError("malloc() failed"); 
        break;
      }
      args->cliSock = cliSock;

      /* Create a new thread to handle the client requests */
      if (pthread_create(&threads[tid++], NULL, HandleRequest, (void *) args)) { 
        WriteError("pthread_create() failed"); 
        break;
      }
      qtdClientes++;
    }
  }
  
  /* Fecha o socket para não receber novas conexões */
  close(srvSock);
  printf("Server will wait for the active threads and terminate!\n");
  /* Wait for all threads to terminate */
  for(i=0; i<tid; i++) {
    pthread_join(threads[i], NULL);
  }
  return 0;
}
