CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lssl -lcrypto

all: protocol_server protocol_client

protocol_server: protocol_server.c
	$(CC) $(CFLAGS) -o protocol_server protocol_server.c $(LDFLAGS)

protocol_client: protocol_client.c
	$(CC) $(CFLAGS) -o protocol_client protocol_client.c $(LDFLAGS)

clean:
	rm -f protocol_server protocol_client
