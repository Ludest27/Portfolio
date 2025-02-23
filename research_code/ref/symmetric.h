#ifndef SYMMETRIC_H
#define SYMMETRIC_H

#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "ascon.h"

typedef ascon_state xof_state;

#define kyber_ascon128_absorb KYBER_NAMESPACE(kyber_ascon128_absorb)
void kyber_ascon128_absorb(ascon_state *s,
                          const uint8_t seed[KYBER_SYMBYTES],
                          uint8_t x,
                          uint8_t y);

#define kyber_ascon256_prf KYBER_NAMESPACE(kyber_ascon256_prf)
void kyber_ascon256_prf(uint8_t *out, size_t outlen, 
                        const uint8_t key[KYBER_SYMBYTES], 
                        uint8_t nonce);
#define kyber_ascon256_rkprf KYBER_NAMESPACE(kyber_ascon256_rkprf)
void kyber_ascon256_rkprf(uint8_t out[KYBER_SSBYTES],
                          const uint8_t key[KYBER_SYMBYTES],
                          const uint8_t input[KYBER_CIPHERTEXTBYTES]);

#define XOF_BLOCKBYTES 8

// Map Kyber functions to ASCON implementations
#define hash_h(OUT, IN, INBYTES) hash_h(OUT, IN, INBYTES)
#define hash_g(OUT, IN, INBYTES) hash_g(OUT, IN, INBYTES)
#define xof_absorb(STATE, SEED, X, Y) kyber_ascon128_absorb(STATE, SEED, X, Y)
#define xof_squeezeblocks(OUT, OUTBLOCKS, STATE) ascon_xof_squeezeblocks(STATE, OUT, OUTBLOCKS)
#define prf(OUT, OUTBYTES, KEY, NONCE) kyber_ascon256_prf(OUT, OUTBYTES, KEY, NONCE)
#define rkprf(OUT, KEY, INPUT) kyber_ascon256_rkprf(OUT, KEY, INPUT)
#endif
