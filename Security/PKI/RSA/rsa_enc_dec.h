#ifndef RSA_ENC_DEC_H
#define RSA_ENC_DEC_H

#include "rsa_types.h"

// Computes (base^exp) mod mod using modular exponentiation
uint modexp(uint base, uint exp, uint mod);

// Encrypt a single 8-bit block using public key (e, n)
uint encrypt_char(unsigned char m, uint e, uint n);

// Decrypt a single block using private key (d, n)
unsigned char decrypt_char(uint c, uint d, uint n);

#endif
