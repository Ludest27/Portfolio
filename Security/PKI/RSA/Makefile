# Makefile for RSA project

CC = gcc
CFLAGS = -Wall -Wextra -O2

# List of executables to build
TARGETS = rsa_keygen rsa_enc rsa_dec

all: $(TARGETS)

rsa_keygen: rsa_keygen.c rsa_keygen_drv.c rsa_keygen.h rsa_types.h
	$(CC) $(CFLAGS) -o rsa_keygen rsa_keygen.c rsa_keygen_drv.c

rsa_enc: rsa_enc_dec.c rsa_enc_drv.c rsa_enc_dec.h rsa_types.h
	$(CC) $(CFLAGS) -o rsa_enc rsa_enc_dec.c rsa_enc_drv.c

rsa_dec: rsa_enc_dec.c rsa_dec_drv.c rsa_enc_dec.h rsa_types.h
	$(CC) $(CFLAGS) -o rsa_dec rsa_enc_dec.c rsa_dec_drv.c

clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean
