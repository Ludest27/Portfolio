#ifndef ASCON_H
#define ASCON_H

#include <stddef.h>
#include <stdint.h>
#define RC0 0xf0
#define RC1 0xe1
#define RC2 0xd2
#define RC3 0xc3
#define RC4 0xb4
#define RC5 0xa5
#define RC6 0x96
#define RC7 0x87
#define RC8 0x78
#define RC9 0x69
#define RCa 0x5a
#define RCb 0x4b



static inline uint64_t LOAD64(const uint8_t* bytes) {
    uint64_t x = 0;
    for(int i = 0; i < 8; i++) {
        x |= ((uint64_t)bytes[i]) << (8 * i);
    }
    return x;
}

static inline void STORE64(uint8_t* bytes, uint64_t x) {
    for(int i = 0; i < 8; i++) {
        bytes[i] = (x >> (8 * i)) & 0xFF;
    }
}

static inline uint64_t LOADBYTES(const uint8_t* bytes, size_t n) {
    uint64_t x = 0;
    for(size_t i = 0; i < n; i++) {
        x |= ((uint64_t)bytes[i]) << (8 * i);
    }
    return x;
}

static inline void STOREBYTES(uint8_t* bytes, uint64_t x, size_t n) {
    for(size_t i = 0; i < n; i++) {
        bytes[i] = (x >> (8 * i)) & 0xFF;
    }
}

// Core state structure
typedef struct {
    uint64_t x[5];
} ascon_state;

// Core functions
void P12(ascon_state* state);

// XOF for matrix generation (SHAKE128 replacement)
void ascon_xof_init(ascon_state* state);
void ascon_xof_absorb(ascon_state* state, const uint8_t* in, size_t inlen);
void ascon_xof_squeeze(ascon_state* state, uint8_t* out, size_t outlen);
void ascon_xof_squeezeblocks(ascon_state* state, uint8_t* out, size_t nblocks);

// CXOF for PRF (SHAKE256 replacement)
void ascon_cxof_init(ascon_state* state);
void ascon_cxof_absorb(ascon_state* state, const uint8_t* in, size_t inlen, uint8_t domain);
void ascon_cxof_squeeze(ascon_state* state, uint8_t* out, size_t outlen);

// Hash functions
void hash_h(uint8_t* out, const uint8_t* in, size_t inlen);        // SHA3-256 replacement (32 bytes)
void hash_g(uint8_t* out, const uint8_t* in, size_t inlen);        // SHA3-512 replacement (64 bytes)

#endif  // ASCON_H
