CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lcrypto

TARGET = ecc
SRC = ecc.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)
