#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>
#include <time.h>
#include "params.h"
#include "ascon.h"
#include "kem.h"

// Function to get current memory usage
static size_t get_memory_usage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}

// Function to measure execution time
static double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static void measure_memory_operations() {
    unsigned char *pk = malloc(KYBER_PUBLICKEYBYTES);
    unsigned char *sk = malloc(KYBER_SECRETKEYBYTES);
    unsigned char *ct = malloc(KYBER_CIPHERTEXTBYTES);
    unsigned char *key = malloc(KYBER_SSBYTES);
    
    if (!pk || !sk || !ct || !key) {
        printf("Memory allocation failed!\n");
        goto cleanup;
    }

    size_t baseline_mem = get_memory_usage();
    double start_time;
    
    printf("\nDynamic Memory Analysis:\n");
    printf("----------------------\n");
    printf("Baseline memory usage: %zu KB\n", baseline_mem);

    // Measure KeyGen
    start_time = get_time();
    size_t before_keygen = get_memory_usage();
    crypto_kem_keypair(pk, sk);
    size_t after_keygen = get_memory_usage();
    double keygen_time = get_time() - start_time;
    
    printf("\nKeyGen:\n");
    printf("- Memory delta: %zd KB\n", after_keygen - before_keygen);
    printf("- Time: %.3f seconds\n", keygen_time);

    // Measure Encapsulation
    start_time = get_time();
    size_t before_encaps = get_memory_usage();
    crypto_kem_enc(ct, key, pk);
    size_t after_encaps = get_memory_usage();
    double encaps_time = get_time() - start_time;
    
    printf("\nEncapsulation:\n");
    printf("- Memory delta: %zd KB\n", after_encaps - before_encaps);
    printf("- Time: %.3f seconds\n", encaps_time);

    // Measure Decapsulation
    start_time = get_time();
    size_t before_decaps = get_memory_usage();
    crypto_kem_dec(key, ct, sk);
    size_t after_decaps = get_memory_usage();
    double decaps_time = get_time() - start_time;
    
    printf("\nDecapsulation:\n");
    printf("- Memory delta: %zd KB\n", after_decaps - before_decaps);
    printf("- Time: %.3f seconds\n", decaps_time);

cleanup:
    free(pk);
    free(sk);
    free(ct);
    free(key);
}

static void measure_state_size() {
    ascon_state state;
    
    printf("\nStatic Memory Analysis:\n");
    printf("---------------------\n");
    printf("Ascon state size: %zu bytes\n", sizeof(state));
    printf("Ascon internal state width: 320 bits (vs Keccak 1600 bits)\n");
    printf("Ascon rate: 64 bits\n");
    printf("Ascon capacity: 256 bits\n");
    
    printf("\nML-KEM Parameters:\n");
    printf("----------------\n");
    printf("KYBER_K: %d\n", KYBER_K);
    printf("KYBER_N: %d\n", KYBER_N);
    printf("KYBER_Q: %d\n", KYBER_Q);
    
    printf("\nKey Sizes:\n");
    printf("----------\n");
    printf("Public key: %d bytes\n", KYBER_PUBLICKEYBYTES);
    printf("Secret key: %d bytes\n", KYBER_SECRETKEYBYTES);
    printf("Ciphertext: %d bytes\n", KYBER_CIPHERTEXTBYTES);
    printf("Shared key: %d bytes\n", KYBER_SSBYTES);
}

int main() {
    printf("Memory Analysis: ML-KEM with Ascon vs Keccak\n");
    printf("==========================================\n");
    
    measure_state_size();
    measure_memory_operations();
    return 0;
}
