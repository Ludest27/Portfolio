#include "rsa_enc_dec.h"

uint modexp(uint base, uint exp, uint mod) {
    uint result = 1;
    base = base % mod;
    while(exp > 0) {
        if(exp & 1)
            result = (result * base) % mod;
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return result;
}

uint encrypt_char(unsigned char m, uint e, uint n) {
    // m is an 8-bit block, output is 16-bit (or more) depending on n
    return modexp((uint)m, e, n);
}

unsigned char decrypt_char(uint c, uint d, uint n) {
    return (unsigned char) modexp(c, d, n);
}
