#include "symmetric.h"
#include"ascon.h"
#include <string.h>

/*************************************************
* Name:        kyber_ascon128_absorb
*
* Description: Absorb step of ASCON-XOF specialized for Kyber context
*
* Arguments:   - ascon_state *state: pointer to state
*              - const uint8_t *seed: pointer to input seed
*              - uint8_t x: additional byte of input
*              - uint8_t y: additional byte of input
**************************************************/
void kyber_ascon128_absorb(ascon_state *state,
                          const uint8_t seed[KYBER_SYMBYTES],
                          uint8_t x,
                          uint8_t y)
{
  uint8_t extseed[KYBER_SYMBYTES+2];
for (size_t i = 0; i < KYBER_SYMBYTES; i++)
    extseed[i] = seed[i];
extseed[KYBER_SYMBYTES] = x;
extseed[KYBER_SYMBYTES+1] = y;
ascon_xof_init(state);
ascon_xof_absorb(state, extseed, KYBER_SYMBYTES);  // Absorb only necessary bytes
ascon_xof_absorb(state, &x, 1);
ascon_xof_absorb(state, &y, 1);

}

/*************************************************
* Name:        kyber_ascon256_prf
*
* Description: Usage of ASCON-CXOF as a PRF
*
* Arguments:   - uint8_t *out: pointer to output
*              - size_t outlen: length of requested output in bytes
*              - const uint8_t *key: pointer to the key
*              - uint8_t nonce: single-byte nonce
**************************************************/
void kyber_ascon256_prf(uint8_t *out,
                        size_t outlen,
                        const uint8_t key[KYBER_SYMBYTES],
                        uint8_t nonce)
{
    ascon_state state;
ascon_cxof_init(&state);
ascon_cxof_absorb(&state, key, KYBER_SYMBYTES, 0x1F);
ascon_cxof_absorb(&state, &nonce, 1, 0x1F);
ascon_cxof_squeeze(&state, out, outlen);

}
/**
 * Name:        kyber_ascon256_rkprf
 *
 * Description: Usage of ASCON-CXOF as a PRF, keyed by `key` and using `input` as input data.
 *
 * Arguments:   - uint8_t *out: pointer to output
 *              - const uint8_t *key: pointer to the key (KYBER_SYMBYTES)
 *              - const uint8_t *input: pointer to the input data (KYBER_CIPHERTEXTBYTES)
 */
void kyber_ascon256_rkprf(uint8_t out[KYBER_SSBYTES],
                          const uint8_t key[KYBER_SYMBYTES],
                          const uint8_t input[KYBER_CIPHERTEXTBYTES])
{
    ascon_state state;
ascon_cxof_init(&state);
ascon_cxof_absorb(&state, key, KYBER_SYMBYTES, 0x1F);
ascon_cxof_absorb(&state, input, KYBER_CIPHERTEXTBYTES, 0x1F);
ascon_cxof_squeeze(&state, out, KYBER_SSBYTES);

}
