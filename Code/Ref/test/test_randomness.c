#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "../ref/ascon.h"

#define SAMPLE_SIZE 10000  // Number of bytes to analyze

// Function to compute entropy
double calculate_entropy(uint8_t *data, size_t length) {
    int freq[256] = {0};
    for (size_t i = 0; i < length; i++) {
        freq[data[i]]++;
    }

    double entropy = 0.0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            double p = (double)freq[i] / length;
            entropy -= p * log2(p);
        }
    }
    return entropy;
}

// Function to compute chi-square test
double chi_square_test(uint8_t *data, size_t length) {
    int observed[256] = {0};
    double expected = length / 256.0;
    double chi_sq = 0.0;

    for (size_t i = 0; i < length; i++) {
        observed[data[i]]++;
    }

    for (int i = 0; i < 256; i++) {
        double diff = observed[i] - expected;
        chi_sq += (diff * diff) / expected;
    }

    return chi_sq;
}

// Function to check bit balance (Frequency Test)
double frequency_test(uint8_t *data, size_t length) {
    int ones = 0, zeros = 0;
    for (size_t i = 0; i < length; i++) {
        for (int bit = 0; bit < 8; bit++) {
            if ((data[i] >> bit) & 1)
                ones++;
            else
                zeros++;
        }
    }
    return (double)ones / (ones + zeros); // Should be close to 0.5
}

int main() {
    uint8_t sample_squeeze[SAMPLE_SIZE];
    uint8_t sample_squeezeblocks[SAMPLE_SIZE];

    ascon_state state1, state2;

    // Initialize both states
    ascon_xof_init(&state1);
    ascon_xof_init(&state2);

    // Generate random data using ascon_xof_squeeze (Full 12 rounds)
    ascon_xof_squeeze(&state1, sample_squeeze, SAMPLE_SIZE);

    // Generate random data using ascon_xof_squeezeblocks (Optimized 8 rounds)
    ascon_xof_squeezeblocks(&state2, sample_squeezeblocks, SAMPLE_SIZE / 8);  // Uses blocks

    // Compute and print statistical results for ascon_xof_squeeze
    printf("===== Randomness Test for ascon_xof_squeeze (12 rounds) =====\n");
    printf("Entropy: %.5f (Ideal: ~8.00)\n", calculate_entropy(sample_squeeze, SAMPLE_SIZE));
    printf("Chi-Square: %.2f (Lower is better)\n", chi_square_test(sample_squeeze, SAMPLE_SIZE));
    printf("Bit Frequency (1s ratio): %.5f (Ideal: 0.5)\n\n", frequency_test(sample_squeeze, SAMPLE_SIZE));

    // Compute and print statistical results for ascon_xof_squeezeblocks
    printf("===== Randomness Test for ascon_xof_squeezeblocks (8 rounds) =====\n");
    printf("Entropy: %.5f (Ideal: ~8.00)\n", calculate_entropy(sample_squeezeblocks, SAMPLE_SIZE));
    printf("Chi-Square: %.2f (Lower is better)\n", chi_square_test(sample_squeezeblocks, SAMPLE_SIZE));
    printf("Bit Frequency (1s ratio): %.5f (Ideal: 0.5)\n", frequency_test(sample_squeezeblocks, SAMPLE_SIZE));

    return 0;
}
