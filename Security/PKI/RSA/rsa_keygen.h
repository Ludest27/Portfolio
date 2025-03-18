#ifndef RSA_KEYGEN_H
#define RSA_KEYGEN_H

#include "rsa_types.h"

// Generates RSA keys using two random 8-bit primes such that n > 256.
// Returns p, q, n, phi (totient), public exponent e, and private exponent d.
void generate_keys(uint *p, uint *q, uint *n, uint *phi, uint *e, uint *d);

#endif
