#include "ascon.h"
#include "params.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#define ASCON_XOF_RATE  8   
#define ASCON_HASH_RATE 8   
#define ASCON_ROUNDS_INIT 12
#define ASCON_ROUNDS_ABSORB 12
#define ASCON_ROUNDS_SQUEEZE 8
#define ASCON_HASH_ROUNDS 12

static const uint64_t ASCON_XOF_IV = 0x00400c0000000100ULL;
static const uint64_t ASCON_CXOF_IV = 0x00400c0000000200ULL;
static const uint64_t ASCON_HASH_IV = 0x00400c0000000300ULL;
static inline uint64_t ROTR(uint64_t x, int n) {
    return (x >> n) | (x << (64 - n));
}

static inline void ROUND(ascon_state* s, uint64_t C) {
    s->x[2] ^= C;
    uint64_t t0 = s->x[0], t1 = s->x[1], t2 = s->x[2], t3 = s->x[3], t4 = s->x[4];
    t0 ^= t4; t4 ^= t3; t2 ^= t1;
    s->x[0] = t0 ^ (~t1 & t2);
    s->x[1] = t1 ^ (~t2 & t3);
    s->x[2] = t2 ^ (~t3 & t4);
    s->x[3] = t3 ^ (~t4 & t0);
    s->x[4] = t4 ^ (~t0 & t1);
    s->x[0] ^= ROTR(t0, 19) ^ ROTR(t0, 28);
    s->x[1] ^= ROTR(t1, 61) ^ ROTR(t1, 39);
    s->x[2] ^= ROTR(t2,  1) ^ ROTR(t2,  6);
    s->x[3] ^= ROTR(t3, 10) ^ ROTR(t3, 17);
    s->x[4] ^= ROTR(t4,  7) ^ ROTR(t4, 41);
}

static void P(ascon_state* s, int rounds) {
    static const uint64_t RC[12] = { 0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 0x78, 0x69, 0x5a, 0x4b };
    for (int i = 12 - rounds; i < 12; i++) {
        ROUND(s, RC[i]);
    }
}
void ascon_xof(uint8_t* out, size_t outlen, const uint8_t* in, size_t inlen) {
    ascon_state state;
    state.x[0] = ASCON_XOF_IV;
    memset(&state.x[1], 0, sizeof(uint64_t) * 4);
    P(&state, ASCON_ROUNDS_INIT);
    while (inlen >= ASCON_XOF_RATE) {
        state.x[0] ^= *(const uint64_t*)in;
        P(&state, ASCON_ROUNDS_ABSORB);
        in += ASCON_XOF_RATE;
        inlen -= ASCON_XOF_RATE;
    }
    uint64_t block = 0;
    memcpy(&block, in, inlen);
    block ^= 0x80ULL << (inlen * 8);
    state.x[0] ^= block;
    P(&state, ASCON_ROUNDS_ABSORB);
    while (outlen >= ASCON_XOF_RATE) {
        memcpy(out, &state.x[0], ASCON_XOF_RATE);
        P(&state, ASCON_ROUNDS_SQUEEZE);
        out += ASCON_XOF_RATE;
        outlen -= ASCON_XOF_RATE;
    }
    if (outlen > 0) {
        memcpy(out, &state.x[0], outlen);
    }
}
