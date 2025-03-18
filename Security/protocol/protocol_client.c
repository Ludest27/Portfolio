#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 4443
#define SERVER_IP "127.0.0.1"

int main(void) {
    SSL_CTX *ctx;
    SSL *ssl;
    int server;
    struct sockaddr_in addr;
    char buffer[1024];

    // Initialize OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    // Create SSL context using TLS
    const SSL_METHOD *method = TLS_client_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // Create a TCP socket
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &addr.sin_addr);

    // Connect to the server
    if (connect(server, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        perror("Connect failed");
        close(server);
        exit(EXIT_FAILURE);
    }

    // Create SSL object and attach to the socket
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, server);
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
    } else {
        int bytes = SSL_read(ssl, buffer, sizeof(buffer)-1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("Received: %s\n", buffer);
        }
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(server);
    SSL_CTX_free(ctx);

    return 0;
}
