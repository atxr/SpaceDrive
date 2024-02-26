#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "libmineziper.h"
#include "libmineziper_crypto.h"

#define PORT 8989

int main()
{
  int server_sockfd, client_sockfd;
  int server_len, client_len;
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;

  server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(PORT);
  server_len = sizeof(server_address);
  bind(server_sockfd, (struct sockaddr *) &server_address, server_len);

  // Create a connection queue, ignore child exit details and wait for clients.

  // Handle only one connection at a time for now
  listen(server_sockfd, 1);

  signal(SIGCHLD, SIG_IGN);

  while (1)
  {
    printf("mineziperd waiting for duty\n");

    /* Accept connection. */
    client_len = sizeof(client_address);
    client_sockfd = accept(
        server_sockfd, (struct sockaddr *) &client_address, &client_len);

    /* Fork to create a process for this client and perform a test to see
    whether we're the parent or the child. */
    if (fork() == 0)
    {
      printf("---------------\nConnection received..\n");
      // Get size of buffer
      unsigned int read_size;
      read(client_sockfd, &read_size, sizeof(int));

      char *buffer = malloc(read_size);
      if (!buffer)
      {
        fprintf(
            stderr, "[ERROR] Cannot allocate buffer of size %d\n", read_size);
        exit(1);
      }

      read(client_sockfd, buffer, read_size);
      // TODO WHAT IF WE READ LESS THAN READ_SIZE
      // DO I NEED A LOOP?

      BYTE hash[SHA256_BLOCK_SIZE];
      SHA256_CTX ctx;
      sha256_init(&ctx);
      sha256_update(&ctx, buffer, read_size);
      sha256_final(&ctx, hash);

      bool scan_result = scan_zip(buffer, read_size);
      printf("Scan finished\n---------------\n");

      write(client_sockfd, &hash, SHA256_BLOCK_SIZE);
      write(client_sockfd, &scan_result, sizeof(scan_result));

      close(client_sockfd);
      exit(0);
    }

    // Otherwise, we must be the parent
    else
    {
      close(client_sockfd);
    }
  }
}