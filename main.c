#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define PORT "8080"

int main(void) {

    printf("Creating socket...\n");

    const struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_STREAM, .ai_flags = AI_PASSIVE};
    struct addrinfo *bind_address = NULL;

    // get addrinfo of localhost into bind address
    if (getaddrinfo(NULL, PORT, &hints, &bind_address) < 0) {
        fprintf(stderr, "ERROR: could not get address info at port: %s\n", PORT);
        return 1;
    };

    int socket_listen_fd = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
    if (socket_listen_fd < 0) {
        fprintf(stderr, "ERROR: could not create server socket: %s\n", strerror(errno));
        return 1;
    }

    printf("Binding socket to local address at port %s...\n", PORT);
    if (bind(socket_listen_fd, bind_address->ai_addr, bind_address->ai_addrlen) < 0) {
        fprintf(stderr, "ERROR: could not bind server socket to address %s: %s\n", bind_address->ai_canonname, strerror(errno));
        return 1;
    };

    freeaddrinfo(bind_address);

    printf("Listening on port %s\n", PORT);
    if (listen(socket_listen_fd, 128) < 0) {
        fprintf(stderr, "ERROR: could not listen for connections %s\n", strerror(errno));
        return 1;
    }

    printf("Waiting for connection...\n");

    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    int socket_client_fd = accept(socket_listen_fd, (struct sockaddr *)&client_address, &client_len);
    if (socket_client_fd < 0) {
        fprintf(stderr, "ERROR: could not accept connection from clients: %s\n", strerror(errno));
        return 1;
    }

    printf("Client is connected... ");
    char address_buffer[100];
    getnameinfo((struct sockaddr *)&client_address, client_len, address_buffer, sizeof(address_buffer), NULL, 0, NI_NUMERICHOST);
    printf("%s\n", address_buffer);

    printf("Reading request...\n");
    char request[1024];
    int bytes_received = recvfrom(socket_client_fd, request, 1024, 0, NULL, NULL);
    printf("Received %d bytes.\n", bytes_received);
    printf("%.*s", bytes_received, request);

    printf("Sending response...\n");
    const char *response = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/plain\r\n\r\nLocal time is: ";
    int bytes_sent = send(socket_client_fd, response, strlen(response), 0);
    printf("Sent %d of %zu bytes.\n", bytes_sent, strlen(response));

    time_t timer;
    time(&timer);                         // get current time
    const char *time_msg = ctime(&timer); // convert current time to string
    bytes_sent = send(socket_client_fd, time_msg, strlen(time_msg), 0);
    printf("Sent %d of %zu bytes.\n", bytes_sent, strlen(time_msg));

    printf("Closing connection...\n");
    close(socket_client_fd);

    printf("Closing listening socket...\n");
    close(socket_listen_fd);

    printf("Finished.\n");

    return 0;
}
