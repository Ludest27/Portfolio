CC = gcc
CFLAGS = -m32 -g
INCLUDE = -I./include

SRCS = src/t.c src/queue.c
OBJS = $(SRCS:.c=.o)
ASM = src/ts.s
TARGET = bin/mt

all: $(TARGET)

$(TARGET): $(OBJS) $(ASM)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(ASM)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
