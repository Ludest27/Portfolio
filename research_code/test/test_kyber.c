#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "../ref/kem.h"
#include "../ref/randombytes.h"

#define NTESTS 1000

static int test_keys(void)
{
  uint8_t pk[KYBER_PUBLICKEYBYTES];
  uint8_t sk[KYBER_SECRETKEYBYTES];
  uint8_t ct[KYBER_CIPHERTEXTBYTES];
  uint8_t key_a[KYBER_SSBYTES];
  uint8_t key_b[KYBER_SSBYTES];

  // Alice generates a public key
  crypto_kem_keypair(pk, sk);

  // Bob derives a secret key and creates a response
  crypto_kem_enc(ct, key_b, pk);

  // Alice uses Bob's response to get her shared key
  crypto_kem_dec(key_a, ct, sk);

  if(memcmp(key_a, key_b, KYBER_SSBYTES)) {
    printf("ERROR keys\n");
    return 1;
  }
  return 0;
}

static int test_invalid_sk_a(void)
{
  uint8_t pk[KYBER_PUBLICKEYBYTES];
  uint8_t sk[KYBER_SECRETKEYBYTES];
  uint8_t ct[KYBER_CIPHERTEXTBYTES];
  uint8_t key_a[KYBER_SSBYTES];
  uint8_t key_b[KYBER_SSBYTES];

  // Alice generates a public key
  crypto_kem_keypair(pk, sk);

  // Bob derives a secret key and creates a response
  crypto_kem_enc(ct, key_b, pk);

  // Replace secret key with random values
  randombytes(sk, KYBER_SECRETKEYBYTES);

  // Alice uses Bob's response to get her shared key
  crypto_kem_dec(key_a, ct, sk);

  if(!memcmp(key_a, key_b, KYBER_SSBYTES)) {
    printf("ERROR invalid sk\n");
    return 1;
  }
  return 0;
}

static int test_invalid_ciphertext(void)
{
  uint8_t pk[KYBER_PUBLICKEYBYTES];
  uint8_t sk[KYBER_SECRETKEYBYTES];
  uint8_t ct[KYBER_CIPHERTEXTBYTES];
  uint8_t key_a[KYBER_SSBYTES];
  uint8_t key_b[KYBER_SSBYTES];
  uint8_t b;
  size_t pos;

  do {
    randombytes(&b, sizeof(uint8_t));
  } while(!b);
  randombytes((uint8_t *)&pos, sizeof(size_t));

  // Alice generates a public key
  crypto_kem_keypair(pk, sk);

  // Bob derives a secret key and creates a response
  crypto_kem_enc(ct, key_b, pk);

  // Change some byte in the ciphertext (i.e., encapsulated key)
  ct[pos % KYBER_CIPHERTEXTBYTES] ^= b;

  // Alice uses Bob's response to get her shared key
  crypto_kem_dec(key_a, ct, sk);

  if(!memcmp(key_a, key_b, KYBER_SSBYTES)) {
    printf("ERROR invalid ciphertext\n");
    return 1;
  }
  return 0;
}

int main(void)
{
  unsigned int i;
  int r;

  for(i=0; i<NTESTS; i++) {
    r  = test_keys();
    r |= test_invalid_sk_a();
    r |= test_invalid_ciphertext();
    if(r)
      return 1;
  }

  printf("KYBER_SECRETKEYBYTES:  %d\n", KYBER_SECRETKEYBYTES);
  printf("KYBER_PUBLICKEYBYTES:  %d\n", KYBER_PUBLICKEYBYTES);
  printf("KYBER_CIPHERTEXTBYTES: %d\n", KYBER_CIPHERTEXTBYTES);
    
  return 0;
}
