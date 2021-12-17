#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <assert.h>

#define MAX_SOCKS (64 * 1024)
int sock_for_port[MAX_SOCKS];


int server_accept(int port) {
  int serversock;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  int serversocket;

  // Creating socket file descriptor
  if ((serversock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Forcefully attaching socket to the port
  if (setsockopt(serversock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
        &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  // Forcefully attaching socket to the port
  if (bind(serversock, (struct sockaddr *)&address, sizeof(address))<0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(serversock, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  fprintf(stdout, "Server waiting for client to connect to port %d\n", port);
  if ((serversocket = accept(serversock, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }

  return serversocket;

}

void server_recv(int port) {
  char buff[256];
  ssize_t n;
  if ((n = read(sock_for_port[port], buff, 5)) < 0) {
    perror("read");
    exit(EXIT_FAILURE);
  }
  assert(buff[0] == 'h' && buff[2] == 'l');
  printf("Server received %ld bytes on port %d\n", n, port);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s START_PORT NUM_PORTS\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  int startPort = atoi(argv[1]);
  int numPorts = atoi(argv[2]);

  unsigned port;

  for (port = startPort; port < startPort + numPorts; port++)
    sock_for_port[port] = server_accept(port);

#if 1
  for (port = startPort; port < startPort + numPorts; port++)
    server_recv(port);
#endif

  return EXIT_SUCCESS;
}
