PKI Portfolio Project - README
=================================

Overview:
---------
This project demonstrates a Public Key Infrastructure (PKI) implementation using RSA and ECC.

Project Structure:
------------------
PKI/
  ├── RSA/          - Contains RSA key generation, encryption, and decryption code.
  ├── ECC/          - Contains ECC key generation and digital signature demonstration.
       Makefile  - (Optional)  Makefile to build each PKI projects.
  └── README.txt    - This documentation file.

Build Instructions:
-------------------
To build the PKI projects, navigate into each subdirectory (RSA, ECC) and run:
    make
Alternatively, if a top-level Makefile is provided, run 'make' from the PKI directory.

Usage:
------
Each subproject includes driver programs that demonstrate key generation, encryption/decryption,
and digital signature operations.

Requirements:
-------------
- GCC (or a compatible C compiler)
- OpenSSL development libraries

Notes:
------
These projects are not to be deployed and demonstrate key management and cryptographic operations.
