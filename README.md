# Trading Engine: Core Parser & Memory-Mapped Simulation

## Motivation

I built this project to deepen my practical understanding of C++ and software architecture by exploring a domain heavily connected to low-level systems programming: trading systems. Rather than getting bogged down in complex network protocols right away, this project focuses entirely on efficiently parsing binary market data (NASDAQ ITCH-50) and building a Limit Order Book (LOB) from the ground up. It serves as a hands-on environment to learn about memory layout, optimal data structure selection, hardware profiling (`perf`), and low-level performance tuning.

## Architecture

The engine operates offline as a deterministic simulation of an exchange feed, reading historical market data directly from a local binary file. It is designed to be cross-platform, utilizing OS-specific system calls for both Windows and Linux to achieve maximum I/O performance.

Engine/
├── data/                    # Market data files (e.g., bin files)
├── docs/                    # Project documentation
│   ├── devlogs/             # Formal architectural decision records & images
│   └── field_notes/         # Unfiltered implementation insights and learnings
├── include/                 # Header files
│   ├── Market.h             # Stock directory management definitions
│   ├── messages.h           # Packed message structs matching ITCH-50 format
│   ├── MmapFile.h           # Cross-platform memory mapping abstraction
│   ├── OrderBook.h          # Limit Order Book definitions
│   ├── Parser.h             # Memory-mapped file parser header
│   └── utils.h              # Shared utilities
├── src/                     # Source files
│   ├── main.cpp             # Application entry point and simulation loop
│   ├── Market.cpp           # Stock directory management implementation
│   ├── MmapFile.cpp         # Cross-platform memory mapping implementation
│   ├── OrderBook.cpp        # Limit Order Book implementation
│   └── Parser.cpp           # Parsing logic and iteration
└── CMakeLists.txt           # Root build configuration

## The Interesting Parts

### Zero-Copy File I/O (`mmap` & `MapViewOfFile`)
Initially, the parser relied on standard `std::ifstream::read()` calls, which created a massive disk I/O bottleneck due to constant kernel context switches. To resolve this, the engine now uses memory mapping (`mmap` on Linux, `MapViewOfFile` on Windows) via `#ifdef` abstractions encapsulated in `MmapFile`. The entire binary file is mapped directly into the virtual address space.

### In-Place Parsing and Pointer Arithmetic
By mapping the file, the data is treated as one continuous array in RAM. The parsing loop simply uses pointer arithmetic (`ptr += message_length`) to jump between packets. By using strict memory layouts and `#pragma pack`, C++ structs are overlaid directly onto the raw memory buffer using `reinterpret_cast`. Endianness (converting NASDAQ's big-endian to little-endian) is handled efficiently using `std::byteswap` to leverage dedicated hardware instructions. This completely eliminates intermediate buffers, dynamic allocation, and `memcpy` overhead.

### Market and Limit Order Book (LOB)
* **Market Directory:** To avoid slow string lookups for stock tickers, the `Market` class uses a statically sized `std::vector` (pre-allocated using `constexpr` for ~10,000 tickers) indexed directly by their integer `locate` code.
* **Order Book (Current Baseline):** Currently, the book relies on `std::map` (Red-Black Tree) to keep price levels sorted and `std::unordered_map` for O(1) order lookups. While functionally correct, hardware profiling has revealed that these node-based containers are the current primary bottleneck.

### Hardware Profiling & Cross-Platform Metrics
Dual-booting and testing on both Windows and Linux revealed massive performance shifts. Upgrading to memory-mapped I/O dropped parsing time drastically on both platforms (e.g., Linux execution dropped from ~410s to ~126s) and nearly eliminated the OS-level performance gap. However, Linux `perf` metrics reveal that the CPU now suffers from a ~62.3% cache miss rate and a low 0.19 Instructions Per Cycle (IPC), indicating the CPU is starved waiting for RAM fetches caused by the standard library's node allocations.

## Tech Stack

* **Language:** C++20
* **Platform:** Linux (GCC) & Windows (MSVC)
* **Build System:** CMake
* **Core Mechanisms:** Zero-copy memory mapping (`mmap`), packed structs, pointer arithmetic, hardware profiling (`perf`).

## Building

The project is configured to build on both Linux and Windows using CMake.

# Create the build directory and configure the project (cross-platform)
cmake -B build -DCMAKE_BUILD_TYPE=Release -S .

# Compile the project in Release mode for optimal performance
cmake --build build --config Release

## Running the Simulation

To run the simulation, the engine requires a binary market data file (e.g., a NASDAQ ITCH-50 `.bin` file).

**On Linux:**
./build/engine data/market_data.bin

**On Windows:**
.\build\Release\engine.exe data\market_data.bin

As the simulation processes the file, the engine outputs periodic telemetry regarding parsing throughput and the current state of the order book.

## Future Phases

The memory mapping implementation successfully eliminated the disk I/O bottleneck, but hardware profiling has exposed standard library data structures as the new bottleneck. The next phase of development will focus entirely on **Cache Optimization**. I plan to replace the node-based `std::map` and `std::unordered_map` with cache-friendly, contiguous memory data structures (such as flat arrays or custom B-Trees) to reduce pointer chasing, drastically lower the 62.3% cache miss rate, and improve the overall IPC.