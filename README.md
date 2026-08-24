# NASDAQ ITCH 5.0 Parser

## Overview
This project is a high-performance C++ parser and order book engine for the NASDAQ ITCH 5.0 market data protocol. It is designed to process hundreds of millions of historical market messages with minimal latency, reconstructing the limit order book for active equities.

The project focuses on system-level optimizations, memory alignment, and CPU cache efficiency, demonstrating how algorithmic choices interact with underlying silicon.

## Project Architecture
The codebase is divided into headers and source files, utilizing standard C++ and CMake.

```text
├── include/
│   ├── FlatHashMap.h
│   ├── Market.h
│   ├── messages.h
│   ├── MmapFile.h
│   ├── OrderBook.h
│   ├── Parser.h
│   ├── PriceLevelBook.h
│   └── utils.h
├── src/
│   ├── FlatHashMap.cpp
│   ├── main.cpp
│   ├── Market.cpp
│   ├── MmapFile.cpp
│   ├── OrderBook.cpp
│   └── Parser.cpp
├── .gitignore
└── CMakeLists.txt
```

## Requirements
* C++17 or higher
* CMake 3.10+
* A compatible compiler (GCC, Clang, or MSVC)
* Supported OS: Linux or Windows

## Building the Project
To compile the project with CMake, run the following commands in the root directory:

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```
*(Note: Always build in `Release` mode to ensure maximum execution speed and compiler optimizations).*

## Usage
To run the parser, provide the path to the uncompressed NASDAQ ITCH 5.0 binary file:

```bash
./Nasdaq-itch-parser ../data/08302019.NASDAQ_ITCH50
```

## Development Phases

### Phase 1: Basic Implementation
The initial phase focused on correctly parsing the binary ITCH 5.0 messages and building a functional order book. Data was read from disk using standard `std::ifstream::read()` calls, and order state was maintained using standard library containers (`std::unordered_map` for the global order directory and `std::map` for the price levels). While accurate, this approach was bottlenecked by disk I/O and dynamic memory allocations.

### Phase 2.1: Zero-Copy File I/O with Memory Mapping
To eliminate disk I/O bottlenecks, standard read operations were replaced with memory mapping (`mmap` on Linux, `MapViewOfFile` on Windows).
* The entire 10GB binary file is mapped directly into the process's virtual address space.
* The OS handles paging data from the SSD into RAM.
* Memory is treated as a continuous array, allowing for in-place parsing using pointer arithmetic (`reinterpret_cast`) without intermediate buffers.

This bypassed OS kernel overhead in the main parsing loop, dropping execution time from ~410s to ~126s on Linux, and from ~1236s to ~140s on Windows.

### Phase 2.2: Custom Data Structures and Hardware Optimization
Profiling with Linux `perf` revealed that while disk I/O was solved, CPU cache misses and high instruction counts were the new bottlenecks. The standard library node-based containers caused significant pointer chasing and memory fragmentation.

* **PriceLevelBook:** Replaced `std::map` with a custom order book backed by a continuous `std::vector`. Based on dataset profiling (90th percentile depth of 245 levels), the vector reserves 256 slots by default. This guarantees contiguous memory and zero reallocation overhead for the vast majority of the market. Insertions and deletions use binary search (`std::lower_bound`) to maintain a perfectly sorted array.
* **FlatHashMap:** Replaced `std::unordered_map` with a custom open-addressing hash map. To solve data clustering, it implements Robin Hood hashing (tracking Distance to Initial Bucket, or DIB). Because Robin Hood hashing supports a high load factor (~90%), the map pre-allocates 170 million slots to handle the entire daily order volume with zero dynamic resizing.

By replacing node-based allocations with contiguous memory structures, the total instruction count dropped by more than half. The parser now executes in ~48.6s on Linux and ~59s on Windows.

## Next Steps
The custom data structures successfully resolved the memory access bottlenecks. In the next phase, the project will simulate a network stack. This will deviate slightly from the current local-file processing architecture, but it is necessary to accurately reflect how live trading systems receive and process UDP multicast packets in production.