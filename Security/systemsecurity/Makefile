# Makefile for File Integrity Monitoring Tool

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lcrypto

TARGET = integrity_monitor
SRC = integrity_monitor.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
