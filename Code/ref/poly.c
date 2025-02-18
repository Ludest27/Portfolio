#include <stdint.h>
#include "params.h"
#include "poly.h"
#include "ntt.h"
#include "reduce.h"
#include "cbd.h"
#include "symmetric.h"

void poly_compress(uint8_t r[KYBER_POLYCOMPRESSEDBYTES], const poly *a) {
 unsigned int i,j;
 int16_t u;
 uint32_t d0;
 uint8_t t[8];

#if (KYBER_POLYCOMPRESSEDBYTES == 128)
 for(i=0;i<KYBER_N/8;i++) {
   for(j=0;j<8;j++) {
     u  = a->coeffs[8*i+j];
     u += (u >> 15) & KYBER_Q;
     d0 = u << 4;
     d0 += 1665;
     d0 *= 80635;
     d0 >>= 28;
     t[j] = d0 & 0xf;
   }

   r[0] = t[0] | (t[1] << 4);
   r[1] = t[2] | (t[3] << 4);
   r[2] = t[4] | (t[5] << 4);
   r[3] = t[6] | (t[7] << 4);
   r += 4;
 }
#elif (KYBER_POLYCOMPRESSEDBYTES == 160)
 for(i=0;i<KYBER_N/8;i++) {
   for(j=0;j<8;j++) {
     u  = a->coeffs[8*i+j];
     u += (u >> 15) & KYBER_Q;
     d0 = u << 5;
     d0 += 1664;
     d0 *= 40318;
     d0 >>= 27;
     t[j] = d0 & 0x1f;
   }

   r[0] = (t[0] >> 0) | (t[1] << 5);
   r[1] = (t[1] >> 3) | (t[2] << 2) | (t[3] << 7);
   r[2] = (t[3] >> 1) | (t[4] << 4);
   r[3] = (t[4] >> 4) | (t[5] << 1) | (t[6] << 6);
   r[4] = (t[6] >> 2) | (t[7] << 3);
   r += 5;
 }
#else
#error "KYBER_POLYCOMPRESSEDBYTES needs to be in {128, 160}"
#endif
}

void poly_decompress(poly *r, const uint8_t a[KYBER_POLYCOMPRESSEDBYTES]) {
#if (KYBER_POLYCOMPRESSEDBYTES == 128)
 unsigned int i;
 for(i=0;i<KYBER_N/2;i++) {
   r->coeffs[2*i+0] = (((uint16_t)(a[0] & 15)*KYBER_Q) + 8) >> 4;
   r->coeffs[2*i+1] = (((uint16_t)(a[0] >> 4)*KYBER_Q) + 8) >> 4;
   a += 1;
 }
#elif (KYBER_POLYCOMPRESSEDBYTES == 160)
 unsigned int i,j;
 uint8_t t[8];
 for(i=0;i<KYBER_N/8;i++) {
   t[0] = (a[0] >> 0);
   t[1] = (a[0] >> 5) | (a[1] << 3);
   t[2] = (a[1] >> 2);
   t[3] = (a[1] >> 7) | (a[2] << 1);
   t[4] = (a[2] >> 4) | (a[3] << 4);
   t[5] = (a[3] >> 1);
   t[6] = (a[3] >> 6) | (a[4] << 2);
   t[7] = (a[4] >> 3);
   a += 5;

   for(j=0;j<8;j++)
     r->coeffs[8*i+j] = ((uint32_t)(t[j] & 31)*KYBER_Q + 16) >> 5;
 }
#else
#error "KYBER_POLYCOMPRESSEDBYTES needs to be in {128, 160}"
#endif
}

void poly_tobytes(uint8_t r[KYBER_POLYBYTES], const poly *a) {
 unsigned int i;
 uint16_t t0, t1;

 for(i=0;i<KYBER_N/2;i++) {
   t0  = a->coeffs[2*i];
   t0 += ((int16_t)t0 >> 15) & KYBER_Q;
   t1 = a->coeffs[2*i+1];
   t1 += ((int16_t)t1 >> 15) & KYBER_Q;
   r[3*i+0] = (t0 >> 0);
   r[3*i+1] = (t0 >> 8) | (t1 << 4);
   r[3*i+2] = (t1 >> 4);
 }
}

void poly_frombytes(poly *r, const uint8_t a[KYBER_POLYBYTES]) {
 unsigned int i;
 for(i=0;i<KYBER_N/2;i++) {
   r->coeffs[2*i]   = ((a[3*i+0] >> 0) | ((uint16_t)a[3*i+1] << 8)) & 0xFFF;
   r->coeffs[2*i+1] = ((a[3*i+1] >> 4) | ((uint16_t)a[3*i+2] << 4)) & 0xFFF;
 }
}

void poly_frommsg(poly *r, const uint8_t msg[KYBER_INDCPA_MSGBYTES]) {
 unsigned int i,j;
 int16_t mask;

#if (KYBER_INDCPA_MSGBYTES != KYBER_N/8)
#error "KYBER_INDCPA_MSGBYTES must be equal to KYBER_N/8 bytes!"
#endif

 for(i=0;i<KYBER_N/8;i++) {
   for(j=0;j<8;j++) {
     mask = -(int16_t)((msg[i] >> j)&1);
     r->coeffs[8*i+j] = mask & ((KYBER_Q+1)/2);
   }
 }
}

void poly_tomsg(uint8_t msg[KYBER_INDCPA_MSGBYTES], const poly *a) {
 unsigned int i,j;
 uint32_t t;

 for(i=0;i<KYBER_N/8;i++) {
   msg[i] = 0;
   for(j=0;j<8;j++) {
     t  = a->coeffs[8*i+j];
     t <<= 1;
     t += 1665;
     t *= 80635;
     t >>= 28;
     t &= 1;
     msg[i] |= t << j;
   }
 }
}

void poly_getnoise_eta1(poly *r, const uint8_t seed[KYBER_SYMBYTES], uint8_t nonce) {
 uint8_t buf[KYBER_ETA1*KYBER_N/4];
 prf(buf, sizeof(buf), seed, nonce);
 poly_cbd_eta1(r, buf);
}

void poly_getnoise_eta2(poly *r, const uint8_t seed[KYBER_SYMBYTES], uint8_t nonce) {
 uint8_t buf[KYBER_ETA2*KYBER_N/4];
 prf(buf, sizeof(buf), seed, nonce);
 poly_cbd_eta2(r, buf);
}

void poly_ntt(poly *r) {
 ntt(r->coeffs);
 poly_reduce(r);
}

void poly_invntt_tomont(poly *r) {
 invntt(r->coeffs);
}

void poly_basemul_montgomery(poly *r, const poly *a, const poly *b) {
 unsigned int i;
 for(i=0;i<KYBER_N/4;i++) {
   basemul(&r->coeffs[4*i], &a->coeffs[4*i], &b->coeffs[4*i], zetas[64+i]);
   basemul(&r->coeffs[4*i+2], &a->coeffs[4*i+2], &b->coeffs[4*i+2], -zetas[64+i]);
 }
}

void poly_tomont(poly *r) {
 unsigned int i;
 const int16_t f = (1ULL << 32) % KYBER_Q;
 for(i=0;i<KYBER_N;i++)
   r->coeffs[i] = montgomery_reduce((int32_t)r->coeffs[i]*f);
}

void poly_reduce(poly *r) {
 unsigned int i;
 for(i=0;i<KYBER_N;i++)
   r->coeffs[i] = barrett_reduce(r->coeffs[i]);
}

void poly_add(poly *r, const poly *a, const poly *b) {
 unsigned int i;
 for(i=0;i<KYBER_N;i++)
   r->coeffs[i] = a->coeffs[i] + b->coeffs[i];
}

void poly_sub(poly *r, const poly *a, const poly *b) {
 unsigned int i;
 for(i=0;i<KYBER_N;i++)
   r->coeffs[i] = a->coeffs[i] - b->coeffs[i];
}
