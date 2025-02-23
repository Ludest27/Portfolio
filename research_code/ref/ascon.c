#include "ascon.h"
#include "params.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

// Define ASCON parameters
#define ASCON_XOF_RATE  8   // Bytes for SHAKE128 replacement
#define ASCON_HASH_RATE 8   // Bytes for SHA3-256 replacement
#define ASCON_ROUNDS_INIT 12
#define ASCON_ROUNDS_ABSORB 12
#define ASCON_ROUNDS_SQUEEZE 8
#define ASCON_HASH_ROUNDS 12

// Define Domain Separation Constants
static const uint64_t ASCON_XOF_IV = 0x00400c0000000100ULL;
static const uint64_t ASCON_CXOF_IV = 0x00400c0000000200ULL;
static const uint64_t ASCON_HASH_IV = 0x00400c0000000300ULL;

// Rotate right utility function
static inline uint64_t ROTR(uint64_t x, int n) {
    return (x >> n) | (x << (64 - n));
}

// ASCON round function
// static inline void ROUND(ascon_state* s, uint64_t C) {
//     s->x[2] ^= C;

//     uint64_t t0 = s->x[0], t1 = s->x[1], t2 = s->x[2], t3 = s->x[3], t4 = s->x[4];

//     t0 ^= t4; t4 ^= t3; t2 ^= t1;

//     s->x[0] = t0 ^ (~t1 & t2);
//     s->x[1] = t1 ^ (~t2 & t3);
//     s->x[2] = t2 ^ (~t3 & t4);
//     s->x[3] = t3 ^ (~t4 & t0);
//     s->x[4] = t4 ^ (~t0 & t1);

//     s->x[0] ^= ROTR(t0, 19) ^ ROTR(t0, 28);
//     s->x[1] ^= ROTR(t1, 61) ^ ROTR(t1, 39);
//     s->x[2] ^= ROTR(t2,  1) ^ ROTR(t2,  6);
//     s->x[3] ^= ROTR(t3, 10) ^ ROTR(t3, 17);
//     s->x[4] ^= ROTR(t4,  7) ^ ROTR(t4, 41);
// }

// #########################################
// Second = No loop and no Function
static void P12(ascon_state* s) {
    static const uint64_t RC[12] = {
        0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5,
        0x96, 0x87, 0x78, 0x69, 0x5a, 0x4b
    };

    uint64_t t0, t1, t2, t3, t4;
    for (int i = 0; i < 12; i++) {
        s->x[2] ^= RC[i];

        t0 = s->x[0];
        t1 = s->x[1];
        t2 = s->x[2];
        t3 = s->x[3];
        t4 = s->x[4];

        t0 ^= t4; 
        t4 ^= t3; 
        t2 ^= t1;

        s->x[0] ^= ROTR(t0, 19) ^ ROTR(t0, 28);
        s->x[1] ^= ROTR(t1, 61) ^ ROTR(t1, 39);
        s->x[2] ^= ROTR(t2,  1) ^ ROTR(t2,  6);
        s->x[3] ^= ROTR(t3, 10) ^ ROTR(t3, 17);
        s->x[4] ^= ROTR(t4,  7) ^ ROTR(t4, 41);
    }

    // uint8_t cnt = 12;
    // while (cnt--) {
    //     s->x[2] ^= RC[12 - cnt];

    //     t0 = s->x[0];
    //     t1 = s->x[1];
    //     t2 = s->x[2];
    //     t3 = s->x[3];
    //     t4 = s->x[4];

    //     t0 ^= t4; 
    //     t4 ^= t3; 
    //     t2 ^= t1;

    //     s->x[0] ^= ROTR(t0, 19) ^ ROTR(t0, 28);
    //     s->x[1] ^= ROTR(t1, 61) ^ ROTR(t1, 39);
    //     s->x[2] ^= ROTR(t2,  1) ^ ROTR(t2,  6);
    //     s->x[3] ^= ROTR(t3, 10) ^ ROTR(t3, 17);
    //     s->x[4] ^= ROTR(t4,  7) ^ ROTR(t4, 41);
    // }
}

static void P8(ascon_state* s) {
    static const uint64_t RC[8] = {
        0xb4, 0xa5, 0x96, 0x87, 
        0x78, 0x69, 0x5a, 0x4b
    };
    
    uint64_t t0, t1, t2, t3, t4;

    for (int i = 0; i < 8; i++) {
        s->x[2] ^= RC[i];

        t0 = s->x[0];
        t1 = s->x[1];
        t2 = s->x[2];
        t3 = s->x[3];
        t4 = s->x[4];

        t0 ^= t4; 
        t4 ^= t3; 
        t2 ^= t1;

        s->x[0] ^= ROTR(t0, 19) ^ ROTR(t0, 28);
        s->x[1] ^= ROTR(t1, 61) ^ ROTR(t1, 39);
        s->x[2] ^= ROTR(t2,  1) ^ ROTR(t2,  6);
        s->x[3] ^= ROTR(t3, 10) ^ ROTR(t3, 17);
        s->x[4] ^= ROTR(t4,  7) ^ ROTR(t4, 41);
    }
    // uint8_t cnt = 8;
    // while (cnt--) {
    //     s->x[2] ^= RC[8 - cnt];

    //     t0 = s->x[0];
    //     t1 = s->x[1];
    //     t2 = s->x[2];
    //     t3 = s->x[3];
    //     t4 = s->x[4];

    //     t0 ^= t4; 
    //     t4 ^= t3; 
    //     t2 ^= t1;

    //     s->x[0] ^= ROTR(t0, 19) ^ ROTR(t0, 28);
    //     s->x[1] ^= ROTR(t1, 61) ^ ROTR(t1, 39);
    //     s->x[2] ^= ROTR(t2,  1) ^ ROTR(t2,  6);
    //     s->x[3] ^= ROTR(t3, 10) ^ ROTR(t3, 17);
    //     s->x[4] ^= ROTR(t4,  7) ^ ROTR(t4, 41);
    // }
}

// #########################################
// First = No loop
// static void P12(ascon_state* s) {
//     ROUND(s, 0xf0);
//     ROUND(s, 0xe1);
//     ROUND(s, 0xd2);
//     ROUND(s, 0xc3);
//     ROUND(s, 0xb4);
//     ROUND(s, 0xa5);
//     ROUND(s, 0x96);
//     ROUND(s, 0x87);
//     ROUND(s, 0x78);
//     ROUND(s, 0x69);
//     ROUND(s, 0x5a);
//     ROUND(s, 0x4b);
// }

// static void P8(ascon_state* s) {
//     ROUND(s, 0xb4);
//     ROUND(s, 0xa5);
//     ROUND(s, 0x96);
//     ROUND(s, 0x87);
//     ROUND(s, 0x78);
//     ROUND(s, 0x69);
//     ROUND(s, 0x5a);
//     ROUND(s, 0x4b);
// }

// ASCON permutation
// static void P(ascon_state* s, int rounds) {
//     static const uint64_t RC[12] = {
//         0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5,
//         0x96, 0x87, 0x78, 0x69, 0x5a, 0x4b
//     };
//     for (int i = 12 - rounds; i < 12; i++) {
//         ROUND(s, RC[i]);
//     }
// }

// ASCON hash absorb
void ascon_hash_absorb(ascon_state* state, const uint8_t* in, size_t inlen) {
    while (inlen >= ASCON_HASH_RATE) {
        state->x[0] ^= *(const uint64_t*)in;
        P12(state);
        in += ASCON_HASH_RATE;
        inlen -= ASCON_HASH_RATE;
    }

    // Final padding
    if (inlen > 0 || inlen == 0) {
        uint64_t block = 0;
        memcpy(&block, in, inlen);
        block ^= 0x80ULL << (inlen * 8);
        state->x[0] ^= block;
        P12(state);
    }
}

// ASCON hash squeeze
void ascon_hash_squeeze(ascon_state* state, uint8_t* out, size_t outlen) {
    while (outlen >= ASCON_HASH_RATE) {
        memcpy(out, &state->x[0], ASCON_HASH_RATE);
        P12(state);
        out += ASCON_HASH_RATE;
        outlen -= ASCON_HASH_RATE;
    }
    if (outlen > 0) {
        memcpy(out, &state->x[0], outlen);
    }
}

// ASCON XOF init
void ascon_xof_init(ascon_state* state) {
    state->x[0] = ASCON_XOF_IV;
    memset(&state->x[1], 0, sizeof(uint64_t) * 4);
    P12(state);
}

// ASCON XOF absorb
void ascon_xof_absorb(ascon_state* state, const uint8_t* in, size_t inlen) {
    while (inlen >= ASCON_XOF_RATE) {
        state->x[0] ^= *(const uint64_t*)in;
        P12(state);
        in += ASCON_XOF_RATE;
        inlen -= ASCON_XOF_RATE;
    }

    // Final padding
    if (inlen > 0 || inlen == 0) {
        uint64_t block = 0;
        memcpy(&block, in, inlen);
        block ^= 0x80ULL << (inlen * 8);
        state->x[0] ^= block;
        P12(state);
    }
}

// ASCON XOF squeeze
void ascon_xof_squeeze(ascon_state* state, uint8_t* out, size_t outlen) {
    while (outlen >= ASCON_XOF_RATE) {
        memcpy(out, &state->x[0], ASCON_XOF_RATE);
        P8(state);
        out += ASCON_XOF_RATE;
        outlen -= ASCON_XOF_RATE;
    }
    if (outlen > 0) {
        memcpy(out, &state->x[0], outlen);
    }
}
void ascon_xof_squeezeblocks(ascon_state* state, uint8_t* out, size_t nblocks) {
    while (nblocks > 0) {
        memcpy(out, &state->x[0], ASCON_XOF_RATE);  // Extract block
        P8(state);             // Apply permutation
        out += ASCON_XOF_RATE;
        nblocks--;
    }
}
// ASCON cXOF init
void ascon_cxof_init(ascon_state* state) {
    state->x[0] = ASCON_CXOF_IV;  // Use cXOF-specific IV
    memset(&state->x[1], 0, sizeof(uint64_t) * 4);  // Clear the rest of the state
    P12(state);  // Apply initial permutation
}

// ASCON cXOF absorb
void ascon_cxof_absorb(ascon_state* state, const uint8_t* in, size_t inlen, uint8_t domain) {
    // Absorb input in blocks of ASCON_XOF_RATE
    while (inlen >= ASCON_XOF_RATE) {
        state->x[0] ^= *(const uint64_t*)in;  // XOR input into state
        P12(state);       // Apply permutation
        in += ASCON_XOF_RATE;
        inlen -= ASCON_XOF_RATE;
    }

    // Handle the final block (including padding)
    if (inlen > 0 || inlen == 0) {
        uint64_t block = 0;
        memcpy(&block, in, inlen);           // Copy remaining bytes
        block ^= 0x80ULL << (inlen * 8);     // Apply padding
        state->x[0] ^= block;                // XOR into state
    }

    // XOR domain byte into the state for domain separation
    state->x[0] ^= ((uint64_t)domain) << 56;  // Domain byte in the MSB
    P12(state);            // Final permutation
}

// ASCON cXOF squeeze
void ascon_cxof_squeeze(ascon_state* state, uint8_t* out, size_t outlen) {
    while (outlen >= ASCON_XOF_RATE) {
        memcpy(out, &state->x[0], ASCON_XOF_RATE);  // Output a block
        P12(state);             // Apply permutation
        out += ASCON_XOF_RATE;
        outlen -= ASCON_XOF_RATE;
    }

    // Handle the final partial block
    if (outlen > 0) {
        memcpy(out, &state->x[0], outlen);  // Output remaining bytes
    }
}
// ASCON XOF: One-shot function (absorb + squeeze in one step)
void ascon_xof(uint8_t* out, size_t outlen, const uint8_t* in, size_t inlen) {
    ascon_state state;
    ascon_xof_init(&state);
    ascon_xof_absorb(&state, in, inlen);
    ascon_xof_squeeze(&state, out, outlen);
}

// ASCON CXOF: One-shot function with domain separation
void ascon_cxof(uint8_t* out, size_t outlen, const uint8_t* in, size_t inlen, uint8_t domain) {
    ascon_state state;
    ascon_cxof_init(&state);
    ascon_cxof_absorb(&state, in, inlen, domain);
    ascon_cxof_squeeze(&state, out, outlen);
}
// ASCON hash functions
void hash_h(uint8_t* out, const uint8_t* in, size_t inlen) {
    ascon_state state;
    state.x[0] = ASCON_HASH_IV;
    memset(&state.x[1], 0, sizeof(uint64_t) * 4);
    ascon_hash_absorb(&state, in, inlen);
    ascon_hash_squeeze(&state, out, 32);
}

void hash_g(uint8_t* out, const uint8_t* in, size_t inlen) {
    ascon_state state;
    state.x[0] = ASCON_HASH_IV;
    memset(&state.x[1], 0, sizeof(uint64_t) * 4);
    ascon_hash_absorb(&state, in, inlen);
    ascon_hash_squeeze(&state, out, 64);
}
