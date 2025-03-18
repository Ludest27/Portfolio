#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rsa_keygen.h"

// Extended Euclidean Algorithm to find modular inverse of a modulo m
static uint mod_inverse(uint a, uint m) {
    int m0 = m, x = 1, y = 0;
    if (m == 1)
        return 0;
    while (a > 1) {
        int q = a / m;
        int t = m;
        m = a % m;
        a = t;
        t = y;
        y = x - q * y;
        x = t;
    }
    if (x < 0)
        x += m0;
    return (uint)x;
}

// Returns gcd of a and b
static uint gcd(uint a, uint b) {
    while(b != 0) {
        uint temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// A list of small primes (8-bit primes)
static const uint primes[] = {3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251};
static const int num_primes = sizeof(primes) / sizeof(primes[0]);

void generate_keys(uint *p, uint *q, uint *n, uint *phi, uint *e, uint *d) {
    srand((unsigned int) time(NULL));
    // Pick two distinct primes such that n = p * q > 256
    do {
        *p = primes[rand() % num_primes];
        *q = primes[rand() % num_primes];
    } while (*p == *q || (*p * *q) <= 256);
    
    *n = (*p) * (*q);
    *phi = (*p - 1) * (*q - 1);
    
    // Choose e: 1 < e < phi and gcd(e, phi) == 1
    for (uint candidate = 3; candidate < *phi; candidate += 2) {
        if (gcd(candidate, *phi) == 1) {
            *e = candidate;
            break;
        }
    }
    
    // Compute d as the modular inverse of e modulo phi
    *d = mod_inverse(*e, *phi);
}
