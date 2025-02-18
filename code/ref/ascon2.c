#include "ascon.h"
#include "params.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// Define ASCON parameters 
#define ASCON_XOF_RATE  8   // Bytes for SHAKE128 replacement
#define ASCON_HASH_RATE 8   // Bytes for SHA3-256 replacement
#define ASCON_ROUNDS_INIT 12   // 
#define ASCON_ROUNDS_ABSORB 12  // 
#define ASCON_ROUNDS_SQUEEZE 12 // 
#define ASCON_HASH_ROUNDS 12  // Standard ASCON-Hash256 and ASCON-Hash512 rounds
// Define Domain Separation Constants 
static const uint64_t ASCON_XOF_IV = 0x00400c0000000100ULL; 
static const uint64_t ASCON_CXOF_IV = 0x00400c0000000200ULL; 
static const uint64_t ASCON_HASH_IV = 0x00400c0000000300ULL;

static inline uint64_t ROTR(uint64_t x, int n) {
    return (x >> n) | (x << (64 - n));
}
static void ROUND(ascon_state* s, uint8_t C) {
    s->x[2] ^= C;

    uint64_t t0 = s->x[0], t1 = s->x[1], t2 = s->x[2], t3 = s->x[3], t4 = s->x[4];

    // Minimize dependency stalls
    t0 ^= t4; 
    t4 ^= t3; 
    t2 ^= t1;

    s->x[0] = t0 ^ (~t1 & t2);
    s->x[1] = t1 ^ (~t2 & t3);
    s->x[2] = t2 ^ (~t3 & t4);
    s->x[3] = t3 ^ (~t4 & t0);
    s->x[4] = t4 ^ (~t0 & t1);

    // Reordered rotations for better instruction scheduling
    s->x[0] ^= ROTR(t0, 19) ^ ROTR(t0, 28);
    s->x[1] ^= ROTR(t1, 61) ^ ROTR(t1, 39);
    s->x[2] ^= ROTR(t2,  1) ^ ROTR(t2,  6);
    s->x[3] ^= ROTR(t3, 10) ^ ROTR(t3, 17);
    s->x[4] ^= ROTR(t4,  7) ^ ROTR(t4, 41);
}


static void P(ascon_state* s, int rounds) {
    static const uint8_t RC[12] = {
        0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5,
        0x96, 0x87, 0x78, 0x69, 0x5a, 0x4b
    };
    for (int i = 12 - rounds; i < 12; i++) {
        ROUND(s, RC[i]);
    }
}
void ascon_hash_absorb(ascon_state* state, const uint8_t* in, size_t inlen) {
    while (inlen >= ASCON_HASH_RATE) {
        state->x[0] ^= *(const uint64_t*)in;
        P(state, 12);  // Only permute when full blocks are absorbed
        in += ASCON_HASH_RATE;
        inlen -= ASCON_HASH_RATE;
    }

    // Final padding and last permutation
    if (inlen > 0) {
        uint64_t block = 0;
        memcpy(&block, in, inlen);
        block ^= 0x80ULL << (inlen * 8);
        state->x[0] ^= block;
        P(state, 12);  // One final permutation for security
    }
}
void ascon_hash_squeeze(ascon_state* state, uint8_t* out, size_t outlen) {
    while (outlen >= ASCON_HASH_RATE) {
        memcpy(out, &state->x[0], ASCON_HASH_RATE);
        P(state, 12);  // Only permute between output blocks
        out += ASCON_HASH_RATE;
        outlen -= ASCON_HASH_RATE;
    }

    if (outlen > 0) {
        memcpy(out, &state->x[0], outlen);
    }
}

void ascon_xof_init(ascon_state* state) {
    state->x[0] = ASCON_XOF_IV;
    memset(&state->x[1], 0, sizeof(uint64_t) * 4);
    P(state, ASCON_ROUNDS_INIT);
}

void ascon_xof_absorb(ascon_state* state, const uint8_t* in, size_t inlen) {
    while (inlen >= ASCON_XOF_RATE) {
        state->x[0] ^= *(const uint64_t*)in;  // Direct memory access instead of memcpy
        P(state, ASCON_ROUNDS_ABSORB);
        in += ASCON_XOF_RATE;
        inlen -= ASCON_XOF_RATE;
    }
    
    if (inlen > 0) {
        uint64_t block = 0;
        memcpy(&block, in, inlen);  // Copy only small remaining bytes
        block ^= 0x80ULL << (inlen * 8);  // Padding
        state->x[0] ^= block;
        P(state, ASCON_ROUNDS_ABSORB);
    }
}


void ascon_xof_squeeze(ascon_state* state, uint8_t* out, size_t outlen) {
    while (outlen >= ASCON_XOF_RATE) {
        memcpy(out, &state->x[0], ASCON_XOF_RATE);
        P(state, 8);
        out += ASCON_XOF_RATE;
        outlen -= ASCON_XOF_RATE;
    }
    if (outlen > 0) {
        memcpy(out, &state->x[0], outlen);
    }
}

void ascon_xof_squeezeblocks(ascon_state* state, uint8_t* out, size_t nblocks) {
    while (nblocks > 0) {
        memcpy(out, &state->x[0], ASCON_XOF_RATE);
        P(state, 8);
        out += ASCON_XOF_RATE;
        nblocks--;
    }
}

void ascon_cxof_init(ascon_state* state) {
    state->x[0] = ASCON_CXOF_IV;
    memset(&state->x[1], 0, sizeof(uint64_t) * 4);
    P(state, ASCON_ROUNDS_INIT);
}

void ascon_cxof_absorb(ascon_state* state, const uint8_t* in, size_t inlen, uint8_t domain) {
    while (inlen >= ASCON_XOF_RATE) {
        state->x[0] ^= *(uint64_t*)in;
        P(state, ASCON_ROUNDS_ABSORB);
        in += ASCON_XOF_RATE;
        inlen -= ASCON_XOF_RATE;
    }
    if (inlen > 0) {
        uint64_t block = 0;
        memcpy(&block, in, inlen);
        state->x[0] ^= block;
    }
    state->x[0] ^= ((uint64_t)domain) << 56;
    P(state, ASCON_ROUNDS_ABSORB);
}

void ascon_cxof_squeeze(ascon_state* state, uint8_t* out, size_t outlen) {
    while (outlen >= ASCON_XOF_RATE) {
        memcpy(out, &state->x[0], ASCON_XOF_RATE);
        P(state, ASCON_ROUNDS_SQUEEZE);
        out += ASCON_XOF_RATE;
        outlen -= ASCON_XOF_RATE;
    }
    if (outlen > 0) {
        memcpy(out, &state->x[0], outlen);
    }
}
void hash_h(uint8_t* out, const uint8_t* in, size_t inlen) {
    ascon_state state;
    state.x[0] = ASCON_HASH_IV;
    memset(&state.x[1], 0, sizeof(uint64_t) * 4);

    ascon_hash_absorb(&state, in, inlen);
    ascon_hash_squeeze(&state, out, 32);  // No need for extra P(state, 12) here
}

void hash_g(uint8_t* out, const uint8_t* in, size_t inlen) {
    ascon_state state;
    state.x[0] = ASCON_HASH_IV;
    memset(&state.x[1], 0, sizeof(uint64_t) * 4);

    ascon_hash_absorb(&state, in, inlen);
    ascon_hash_squeeze(&state, out, 64);  // No need for extra P(state, 12) here
}
