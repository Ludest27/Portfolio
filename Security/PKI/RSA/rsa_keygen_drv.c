#include <stdio.h>
#include "rsa_keygen.h"

int main(void) {
    uint p, q, n, phi, e, d;
    generate_keys(&p, &q, &n, &phi, &e, &d);
    printf("Generated RSA keys:\n");
    printf("p: %u\n", p);
    printf("q: %u\n", q);
    printf("n: %u\n", n);
    printf("phi: %u\n", phi);
    printf("e (public key): %u\n", e);
    printf("d (private key): %u\n", d);
    return 0;
}
