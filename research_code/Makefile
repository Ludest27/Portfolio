CC ?= gcc
CFLAGS += -Wall -Wextra -Wpedantic -O3 -fomit-frame-pointer
INCLUDES = -Iref -Itest

# Source files for the library
LIB_SOURCES = ref/ascon.c ref/cbd.c ref/indcpa.c ref/kem.c ref/ntt.c \
              ref/poly.c ref/polyvec.c ref/randombytes.c ref/reduce.c \
              ref/symmetric_ascon.c ref/verify.c

# Source files for tests
TEST_SOURCES = test/test_basic.c test/test_kyber.c test/test_speed.c \
               test/cpucycles.c test/speed_print.c test/test_iot_memory.c\


# Object files
LIB_OBJECTS = $(LIB_SOURCES:.c=.o)
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)

# Test binaries
TEST_BINS = test/test_basic test/test_kyber test/test_speed test/test_memory test/test_iot_memory

.PHONY: all clean test

all: lib test

# Library target
lib: ref/libmlkem.a

ref/libmlkem.a: $(LIB_OBJECTS)
        ar rcs $@ $(LIB_OBJECTS)

# Test targets
test: $(TEST_BINS)

test/test_basic: test/test_basic.o ref/libmlkem.a
        $(CC) $(CFLAGS) -o $@ $< -Lref -lmlkem

test/test_kyber: test/test_kyber.o ref/libmlkem.a
        $(CC) $(CFLAGS) -o $@ $< -Lref -lmlkem

test/test_speed: test/test_speed.o test/cpucycles.o test/speed_print.o ref/libmlkem.a
        $(CC) $(CFLAGS) -o $@ test/test_speed.o test/cpucycles.o test/speed_print.o -Lref -lmlkem

test/test_memory: test/test_memory.o ref/libmlkem.a
        $(CC) $(CFLAGS) -o $@ $< -Lref -lmlkem
test/test_iot_memory: test/test_iot_memory.o ref/libmlkem.a
        $(CC) $(CFLAGS) -o $@ $< -Lref -lmlkem
# Generic rules
%.o: %.c
        $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
        rm -f $(LIB_OBJECTS) $(TEST_OBJECTS) $(TEST_BINS) ref/libmlkem.a test/test_memory.o test/test_iot_memory.o  test/test_basic.o test/test_kyber.o test/test_speed.o
