#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "params.h"
#include "ascon.h"
#include "kem.h"
#define SENSOR_RAM_SIZE (4 * 1024)     // 4KB RAM
#define SENSOR_FLASH_SIZE (32 * 1024)  // 32KB Flash
typedef struct {
    size_t flash_usage;
    size_t static_memory;
    size_t temp_buffer_size;
    double operation_time;
} iot_metrics_t;
static size_t track_temp_buffers() {
    return (KYBER_N * sizeof(int16_t)) + (KYBER_SSBYTES) + sizeof(ascon_state);
}
// Measure execution time and memory usage for a given operation
static iot_metrics_t test_iot_operation(const char* operation_name) {
    iot_metrics_t metrics = {0};
    struct timespec time_start, time_end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &time_start);

    if (strcmp(operation_name, "keygen") == 0) {
        unsigned char pk[KYBER_PUBLICKEYBYTES], sk[KYBER_SECRETKEYBYTES];
        crypto_kem_keypair(pk, sk);
        metrics.static_memory = sizeof(pk) + sizeof(sk);
    }
    else if (strcmp(operation_name, "encaps") == 0) {
        unsigned char pk[KYBER_PUBLICKEYBYTES], ct[KYBER_CIPHERTEXTBYTES], ss[KYBER_SSBYTES];
        crypto_kem_enc(ct, ss, pk);
        metrics.static_memory = sizeof(pk) + sizeof(ct) + sizeof(ss);
    }
    else if (strcmp(operation_name, "decaps") == 0) {
        unsigned char sk[KYBER_SECRETKEYBYTES], ct[KYBER_CIPHERTEXTBYTES], ss[KYBER_SSBYTES];
        crypto_kem_dec(ss, ct, sk);
        metrics.static_memory = sizeof(sk) + sizeof(ct) + sizeof(ss);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &time_end);
    metrics.flash_usage = metrics.static_memory;
    metrics.temp_buffer_size = track_temp_buffers();
    metrics.operation_time = (time_end.tv_sec - time_start.tv_sec) +
                             (time_end.tv_nsec - time_start.tv_nsec) / 1e9;
    return metrics;
}

// Print IoT memory and execution time summary
static void print_iot_summary(const char* operation, iot_metrics_t metrics) {
    printf("\nIoT Analysis for %s:\n", operation);
    printf("Flash required: %zu bytes\n", metrics.flash_usage);
    printf("Static memory: %zu bytes\n", metrics.static_memory);
    printf("Temp buffers: %zu bytes\n", metrics.temp_buffer_size);
    printf("Execution time: %.6f sec\n", metrics.operation_time);
}

int main() {
    printf("Running IoT Memory Analysis...\n");
    iot_metrics_t keygen_metrics = test_iot_operation("keygen");
    print_iot_summary("Key Generation", keygen_metrics);
    iot_metrics_t encaps_metrics = test_iot_operation("encaps");
    print_iot_summary("Encapsulation", encaps_metrics);
    iot_metrics_t decaps_metrics = test_iot_operation("decaps");
    print_iot_summary("Decapsulation", decaps_metrics);

    return 0;
}
