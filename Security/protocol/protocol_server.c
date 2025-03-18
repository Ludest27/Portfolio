#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 4443
#define CERT_FILE "server.crt"
#define KEY_FILE  "server.key"

int main(void) {
    SSL_CTX *ctx;
    SSL *ssl;
    int server_sock, client_sock;
    struct sockaddr_in addr;
    int opt = 1;
    socklen_t addr_len = sizeof(addr);

    // Initialize OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    // Create SSL context using TLS
    const SSL_METHOD *method = TLS_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // Load certificate and private key files
    if (SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "Private key does not match the certificate\n");
        exit(EXIT_FAILURE);
    }

    // Create a TCP socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    // Allow socket address reuse
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind socket to PORT on all interfaces
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 1) < 0) {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }
    printf("TLS server listening on port %d\n", PORT);

    // Accept a client connection
    client_sock = accept(server_sock, (struct sockaddr*)&addr, &addr_len);
    if (client_sock < 0) {
        perror("Unable to accept");
        exit(EXIT_FAILURE);
    }

    // Create an SSL object and attach it to the client socket
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_sock);
    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
    } else {
        const char reply[] = "Hello from TLS server!";
        SSL_write(ssl, reply, strlen(reply));
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client_sock);
    close(server_sock);
    SSL_CTX_free(ctx);

    return 0;
}
