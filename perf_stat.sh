#!/bin/bash

perf stat -B -e cache-references,cache-misses,L1-dcache-load-misses,L1-dcache-store-misses,L1-dcache-prefetch-misses,L1-icache-load-misses,L1-icache-prefetch-misses,LLC-load-misses,LLC-store-misses,LLC-prefetch-misses,cycles,instructions,branches,faults,migrations $*