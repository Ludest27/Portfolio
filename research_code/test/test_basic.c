#include <stdio.h>
#include <string.h>
#include "../ref/params.h"
#include "../ref/kem.h"

// Utility function to print bytes in hex
void print_bytes(const char* label, const unsigned char* bytes, size_t len) {
    printf("%s: ", label);
    for(size_t i = 0; i < len; i++) {
        printf("%02x", bytes[i]);
    }
    printf("\n");
}

int test_key_generation() {
    printf("\nTesting Key Generation...\n");
    
    unsigned char pk[KYBER_PUBLICKEYBYTES];
    unsigned char sk[KYBER_SECRETKEYBYTES];
    
    // Generate key pair
    if (crypto_kem_keypair(pk, sk) != 0) {
        printf("ERROR: Key generation failed\n");
        return -1;
    }
    
    print_bytes("Public Key (first 32 bytes)", pk, 32);
    print_bytes("Secret Key (first 32 bytes)", sk, 32);
    printf("Key generation successful!\n");
    return 0;
}

int test_encaps_decaps() {
    printf("\nTesting Encapsulation/Decapsulation...\n");
    
    unsigned char pk[KYBER_PUBLICKEYBYTES];
    unsigned char sk[KYBER_SECRETKEYBYTES];
    unsigned char ct[KYBER_CIPHERTEXTBYTES];
    unsigned char ss1[KYBER_SSBYTES];  // Shared secret from encaps
    unsigned char ss2[KYBER_SSBYTES];  // Shared secret from decaps
    
    // Generate keys
    if (crypto_kem_keypair(pk, sk) != 0) {
        printf("ERROR: Key generation failed\n");
        return -1;
    }
    
    // Encapsulate
    if (crypto_kem_enc(ct, ss1, pk) != 0) {
        printf("ERROR: Encapsulation failed\n");
        return -1;
    }
    
    // Decapsulate
    if (crypto_kem_dec(ss2, ct, sk) != 0) {
        printf("ERROR: Decapsulation failed\n");
        return -1;
    }
    
    // Verify shared secrets match
    if (memcmp(ss1, ss2, KYBER_SSBYTES) != 0) {
        printf("ERROR: Shared secrets do not match!\n");
        print_bytes("ss1", ss1, KYBER_SSBYTES);
        print_bytes("ss2", ss2, KYBER_SSBYTES);
        return -1;
    }
    
    printf("Encaps/Decaps successful - shared secrets match!\n");
    print_bytes("Shared Secret", ss1, KYBER_SSBYTES);
    return 0;
}

int main() {
    printf("Starting basic ML-KEM functionality tests...\n");
    
    // Test key generation
    if (test_key_generation() != 0) {
        return 1;
    }
    
    // Test encaps/decaps
    if (test_encaps_decaps() != 0) {
        return 1;
    }
    
    printf("\nAll tests passed successfully!\n");
    return 0;
}
