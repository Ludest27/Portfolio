#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/err.h>

int main(void) {
    EVP_PKEY_CTX *pctx = NULL;
    EVP_PKEY *pkey = NULL;
    int ret;

    /* --- ECC Key Generation --- */
    pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
    if (!pctx) {
        fprintf(stderr, "Error creating ECC keygen context.\n");
        return 1;
    }
    if (EVP_PKEY_keygen_init(pctx) <= 0) {
        fprintf(stderr, "Error initializing ECC keygen.\n");
        EVP_PKEY_CTX_free(pctx);
        return 1;
    }
    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_X9_62_prime256v1) <= 0) {
        fprintf(stderr, "Error setting ECC curve.\n");
        EVP_PKEY_CTX_free(pctx);
        return 1;
    }
    ret = EVP_PKEY_keygen(pctx, &pkey);
    EVP_PKEY_CTX_free(pctx);
    if (ret <= 0) {
        fprintf(stderr, "Error generating ECC key pair.\n");
        return 1;
    }

    /* Save private key */
    FILE *f = fopen("ecc_private.pem", "w");
    if (!f) {
        fprintf(stderr, "Unable to open file for ECC private key.\n");
        EVP_PKEY_free(pkey);
        return 1;
    }
    if (!PEM_write_PrivateKey(f, pkey, NULL, NULL, 0, NULL, NULL)) {
        fprintf(stderr, "Error writing ECC private key.\n");
        fclose(f);
        EVP_PKEY_free(pkey);
        return 1;
    }
    fclose(f);

    /* Save public key */
    f = fopen("ecc_public.pem", "w");
    if (!f) {
        fprintf(stderr, "Unable to open file for ECC public key.\n");
        EVP_PKEY_free(pkey);
        return 1;
    }
    if (!PEM_write_PUBKEY(f, pkey)) {
        fprintf(stderr, "Error writing ECC public key.\n");
        fclose(f);
        EVP_PKEY_free(pkey);
        return 1;
    }
    fclose(f);
    printf("ECC key pair generated and saved to ecc_private.pem and ecc_public.pem\n");

    /* --- Signing --- */
    const char *message = "Hello, ECC PKI";
    size_t message_len = strlen(message);
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        fprintf(stderr, "Error creating MD context for signing.\n");
        EVP_PKEY_free(pkey);
        return 1;
    }
    if (EVP_DigestSignInit(mdctx, NULL, EVP_sha256(), NULL, pkey) <= 0) {
        fprintf(stderr, "Error initializing digest sign.\n");
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pkey);
        return 1;
    }
    if (EVP_DigestSignUpdate(mdctx, message, message_len) <= 0) {
        fprintf(stderr, "Error during DigestSignUpdate.\n");
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pkey);
        return 1;
    }
    size_t sig_len = 0;
    if (EVP_DigestSignFinal(mdctx, NULL, &sig_len) <= 0) {
        fprintf(stderr, "Error obtaining signature length.\n");
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pkey);
        return 1;
    }
    unsigned char *signature = OPENSSL_malloc(sig_len);
    if (!signature) {
        fprintf(stderr, "Memory allocation error for signature.\n");
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pkey);
        return 1;
    }
    if (EVP_DigestSignFinal(mdctx, signature, &sig_len) <= 0) {
        fprintf(stderr, "Error during DigestSignFinal.\n");
        OPENSSL_free(signature);
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pkey);
        return 1;
    }
    EVP_MD_CTX_free(mdctx);

    printf("Signature (%zu bytes): ", sig_len);
    for (size_t i = 0; i < sig_len; i++) {
        printf("%02x", signature[i]);
    }
    printf("\n");

    /* --- Verification --- */
    mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        fprintf(stderr, "Error creating MD context for verification.\n");
        OPENSSL_free(signature);
        EVP_PKEY_free(pkey);
        return 1;
    }
    if (EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, pkey) <= 0) {
        fprintf(stderr, "Error initializing digest verify.\n");
        EVP_MD_CTX_free(mdctx);
        OPENSSL_free(signature);
        EVP_PKEY_free(pkey);
        return 1;
    }
    if (EVP_DigestVerifyUpdate(mdctx, message, message_len) <= 0) {
        fprintf(stderr, "Error during DigestVerifyUpdate.\n");
        EVP_MD_CTX_free(mdctx);
        OPENSSL_free(signature);
        EVP_PKEY_free(pkey);
        return 1;
    }
    ret = EVP_DigestVerifyFinal(mdctx, signature, sig_len);
    EVP_MD_CTX_free(mdctx);
    if (ret == 1) {
        printf("Signature verified successfully.\n");
    } else if (ret == 0) {
        printf("Signature verification failed.\n");
    } else {
        fprintf(stderr, "Error during signature verification.\n");
    }

    OPENSSL_free(signature);
    EVP_PKEY_free(pkey);
    return 0;
}
