#!/bin/bash

echo "Running IoT-focused Memory Analysis"
echo "================================="

# Compile with debug info for better analysis
make clean
CFLAGS="-g -O0" make test/test_iot_memory

# Run basic test
./test/test_iot_memory

# Analyze memory access patterns
valgrind --tool=massif \
         --stacks=yes \
         --heap-admin=8 \
         --threshold=0.1 \
         --massif-out-file=iot_memory.out \
         ./test/test_iot_memory

# Generate memory profile
ms_print iot_memory.out > iot_memory_profile.txt

# Check peak memory
echo -e "\nPeak Memory Analysis:"
grep -A 5 "Peak Memory" iot_memory_profile.txt
