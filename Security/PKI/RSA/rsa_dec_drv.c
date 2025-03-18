#include <stdio.h>
#include <stdlib.h>
#include "rsa_enc_dec.h"
#include "rsa_types.h"

int main(int argc, char *argv[]) {
    if(argc < 4) {
        printf("Usage: %s <n> <d> <ciphertext_numbers...>\n", argv[0]);
        return 1;
    }
    uint n = (uint) atoi(argv[1]);
    uint d = (uint) atoi(argv[2]);
    
    printf("Private key: n=%u, d=%u\n", n, d);
    printf("Ciphertext: ");
    for (int i = 3; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\nDecrypted text: ");
    for (int i = 3; i < argc; i++) {
        uint cipher = (uint) atoi(argv[i]);
        unsigned char m = decrypt_char(cipher, d, n);
        printf("%c", m);
    }
    printf("\n");
    return 0;
}
