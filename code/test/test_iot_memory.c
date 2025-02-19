#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <time.h>
#include "params.h"
#include "ascon.h"
#include "kem.h"

// IoT Constraints (Ultra-Low Power MCU)
#define SENSOR_RAM_SIZE (4 * 1024)     // 4KB RAM
#define SENSOR_FLASH_SIZE (32 * 1024)  // 32KB Flash

typedef struct {
    size_t flash_usage;
    size_t static_memory;
    size_t temp_buffer_size;
    double operation_time;
} iot_metrics_t;

// Temporary Buffer Tracking
static size_t track_temp_buffers() {
    size_t temp_size = 0;
    temp_size += ( KYBER_N * sizeof(int16_t));  // Two NTT buffers
    temp_size += (KYBER_N * sizeof(int16_t));  // Polynomial multiplication
    temp_size += sizeof(ascon_state);             // Ascon hash/XOF state
    temp_size += KYBER_SSBYTES;                   // Secret shared buffer
    return temp_size;
}

// Measure execution time and memory usage for a given operation
static iot_metrics_t test_iot_operation(const char* operation_name) {
    iot_metrics_t metrics = {0};
    struct timespec time_start, time_end;

    // Start timing
    clock_gettime(CLOCK_MONOTONIC_RAW, &time_start);

    // Perform KEM operation
    if (strcmp(operation_name, "keygen") == 0) {
        unsigned char *pk = malloc(KYBER_PUBLICKEYBYTES);
        unsigned char *sk = malloc(KYBER_SECRETKEYBYTES);
        if (!pk || !sk) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        crypto_kem_keypair(pk, sk);
        metrics.static_memory = KYBER_PUBLICKEYBYTES + KYBER_SECRETKEYBYTES;
        free(pk);
        free(sk);
    }
    else if (strcmp(operation_name, "encaps") == 0) {
        unsigned char *pk = malloc(KYBER_PUBLICKEYBYTES);
        unsigned char *ct = malloc(KYBER_CIPHERTEXTBYTES);
        unsigned char *ss = malloc(KYBER_SSBYTES);
        if (!pk || !ct || !ss) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        crypto_kem_enc(ct, ss, pk);
        metrics.static_memory = KYBER_PUBLICKEYBYTES + KYBER_CIPHERTEXTBYTES + KYBER_SSBYTES;
        free(pk);
        free(ct);
        free(ss);
    }
    else if (strcmp(operation_name, "decaps") == 0) {
        unsigned char *sk = malloc(KYBER_SECRETKEYBYTES);
        unsigned char *ct = malloc(KYBER_CIPHERTEXTBYTES);
        unsigned char *ss = malloc(KYBER_SSBYTES);
        if (!sk || !ct || !ss) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        crypto_kem_dec(ss, ct, sk);
        metrics.static_memory = KYBER_SECRETKEYBYTES + KYBER_CIPHERTEXTBYTES + KYBER_SSBYTES;
        free(sk);
        free(ct);
        free(ss);
    }

    // End timing
    clock_gettime(CLOCK_MONOTONIC_RAW, &time_end);

    // Compute Accurate Metrics
    metrics.flash_usage = metrics.static_memory;  // Use actual allocated memory, NOT pointer size
    metrics.temp_buffer_size = track_temp_buffers();
    metrics.operation_time = (time_end.tv_sec - time_start.tv_sec) +
                             (time_end.tv_nsec - time_start.tv_nsec) / 1e9;

    return metrics;
}

// Print IoT Analysis Summary
static void print_iot_summary(const char* operation, iot_metrics_t metrics) {
    printf("\nIoT Analysis for %s:\n", operation);
    printf("------------------------\n");
    printf("Flash required: %zu bytes\n", metrics.flash_usage);
    printf("Static memory: %zu bytes\n", metrics.static_memory);
    printf("Temp buffers: %zu bytes\n", metrics.temp_buffer_size);
    printf("Execution time: %.6f sec\n", metrics.operation_time);

    // IoT Constraints Check
    printf("\nIoT Constraints Check:\n");
    printf("Flash usage: %zu/%d bytes\n", metrics.flash_usage, SENSOR_FLASH_SIZE);

    if (metrics.flash_usage > SENSOR_FLASH_SIZE * 0.9) {
        printf(" Flash usage is reaching the limit (%.2f%% used)!\n",
              (metrics.flash_usage / (double)SENSOR_FLASH_SIZE) * 100);
    }

    printf("RAM usage: %zu/%d bytes\n", metrics.temp_buffer_size, SENSOR_RAM_SIZE);

    if (metrics.temp_buffer_size > SENSOR_RAM_SIZE) {
        printf("ERROR: RAM usage exceeds IoT device capacity by %zu bytes!\n",
              metrics.temp_buffer_size - SENSOR_RAM_SIZE);
    } else if (metrics.temp_buffer_size > SENSOR_RAM_SIZE * 0.9) {
        printf("WARNING: High RAM usage (%.2f%% of limit reached)!\n",
              (metrics.temp_buffer_size / (double)SENSOR_RAM_SIZE) * 100);
    }
}

// Main Function
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
