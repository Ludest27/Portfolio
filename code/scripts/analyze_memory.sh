#!/bin/bash

echo "Running Comprehensive Memory Analysis"
echo "==================================="

# Compile with debug info
make clean
CFLAGS="-g -O0" make test/test_memory

# Run basic analysis
echo -e "\n1. Basic Memory Usage:"
size test/test_memory

# Run with valgrind for detailed memory analysis
echo -e "\n2. Valgrind Memory Analysis:"
valgrind --tool=massif \
         --stacks=yes \
         --heap-admin=8 \
         --massif-out-file=massif.out \
         ./test/test_memory

# Generate heap profile graph
ms_print massif.out > memory_profile.txt

# Run with cachegrind for cache analysis
echo -e "\n3. Cache Analysis:"
valgrind --tool=cachegrind \
         --branch-sim=yes \
         ./test/test_memory

# Check for memory leaks
echo -e "\n4. Memory Leak Check:"
valgrind --tool=memcheck \
         --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         ./test/test_memory

# Analyze binary sections
echo -e "\n5. Binary Analysis:"
readelf -S test/test_memory
