#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rsa_enc_dec.h"
#include "rsa_types.h"

int main(int argc, char *argv[]) {
    if(argc < 4) {
        printf("Usage: %s <n> <e> <plaintext>\n", argv[0]);
        return 1;
    }
    uint n = (uint) atoi(argv[1]);
    uint e = (uint) atoi(argv[2]);
    char *plaintext = argv[3];
    
    printf("Public key: n=%u, e=%u\n", n, e);
    printf("Plaintext: %s\n", plaintext);
    printf("Ciphertext: ");
    for(size_t i = 0; i < strlen(plaintext); i++) {
        uint cipher = encrypt_char((unsigned char)plaintext[i], e, n);
        printf("%u ", cipher);
    }
    printf("\n");
    return 0;
}
