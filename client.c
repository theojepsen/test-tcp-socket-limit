#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAX_PORTS (64 * 1024)
int sock_for_port[MAX_PORTS];

void client_send(int port) {
  const char buf[] = {'h', 'e', 'l', 'l', 'o'};
  ssize_t n;
  if ((n = write(sock_for_port[port], buf, sizeof(buf))) < 0) {
    perror("write");
    exit(EXIT_FAILURE);
  }
  printf("Sent %ld bytes to port %d\n", n, port);
}

int client_connect(char *serverip, int port) {
  int clientsock, opt = 1;
  struct sockaddr_in servaddr;

  fprintf(stdout, "Connecting to %s:%d\n", serverip, port);

  if ((clientsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(clientsock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

#if 0
  struct sockaddr_in clientaddr;
  clientaddr.sin_family = AF_INET;
  clientaddr.sin_addr.s_addr = INADDR_ANY;
  clientaddr.sin_port = htons(port);
  if (bind(clientsock, (struct sockaddr *)&clientaddr, sizeof(clientaddr))<0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }
#endif

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);

  if (inet_pton(AF_INET, serverip, &servaddr.sin_addr) <= 0) {
    fprintf(stderr, "INVALID ADDR\n");
    exit(EXIT_FAILURE);
  }

  usleep(10000);
  while (connect(clientsock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "CONNECTION FAILED, retrying...\n");
    sleep(1);
  }

  return clientsock;
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s SERVER_IP START_PORT NUM_PORTS\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char *serverip = argv[1];
  int startPort = atoi(argv[2]);
  int numPorts = atoi(argv[3]);

  unsigned port;

  for (port = startPort; port < startPort + numPorts; port++)
    sock_for_port[port] = client_connect(serverip, port);

#if 1
  for (port = startPort; port < startPort + numPorts; port++)
    client_send(port);
#endif

  return EXIT_SUCCESS;
}
