TLS/SSL Protocol Project - README
===================================

Overview:
---------
This project demonstrates secure network communication using TLS/SSL.
It includes a TLS server and a matching client implemented in C with OpenSSL.

Project Structure:
------------------
Protocol/
  ├── protocol_server.c  - TLS server code.
  ├── protocol_client.c  - TLS client code.
  ├── Makefile           - Build instructions for the server and client.
  └── README.txt         - This documentation file.

Certificate Generation:
-------------------------
Before running the server, generate a self-signed certificate and private key:

    openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout server.key -out server.crt -subj "/CN=localhost"

Place the generated server.key and server.crt files in the Protocol directory.

Build Instructions:
-------------------
In the Protocol directory, run:
    make
This will build the executables: protocol_server and protocol_client.

Usage:
------
1. Start the TLS server:
      ./protocol_server
2. In a separate terminal, run the TLS client:
      ./protocol_client
The client will connect to the server on port 4443, perform a TLS handshake, and display the server's message.

Requirements:
-------------
- GCC
- OpenSSL development libraries

Notes:
------
This project is for demonstration purposes. In production,  certificates from a trusted CA and
 additional security measures are needed.
