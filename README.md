# Trading Engine: Core Parser & Memory-Mapped Simulation

## Motivation

I built this project to dive deep into C++ systems programming and low-level performance optimization.
Instead of tackling complex networking right away, I wanted to focus on the core mechanics of high-throughput data processing by building a NASDAQ ITCH-50 parser and a Limit Order Book (LOB) from scratch.
It is a practical playground for exploring memory layouts, data structure trade-offs, and hardware bottlenecks without relying on heavy external frameworks.

## Architecture

The engine runs offline as a deterministic simulation, processing historical market data from a local binary file. It is built to be cross-platform,
using OS-specific system calls for both Windows and Linux to handle heavy I/O efficiently.

```text
Engine/
├── data/                    # Market data files (e.g., ITCH-50 .bin files)
├── docs/                    # Project documentation
│   ├── devlogs/             # Formal architectural decision records & images
│   └── field_notes/         # Unfiltered implementation insights and learnings
├── include/                 # Header files
│   ├── FlatHashMap.h        # Robin Hood open-addressing map definition
│   ├── Market.h             # Stock directory management definitions
│   ├── messages.h           # Packed message structs matching ITCH-50 format
│   ├── MmapFile.h           # Cross-platform memory mapping abstraction
│   ├── OrderBook.h          # Limit Order Book definitions
│   ├── Parser.h             # Memory-mapped file parser header
│   ├── PriceLevelBook.h     # Contiguous memory order book definitions
│   └── utils.h              # Shared utilities
├── src/                     # Source files
│   ├── FlatHashMap.cpp      # Robin Hood hash map implementation
│   ├── main.cpp             # Application entry point and simulation loop
│   ├── Market.cpp           # Stock directory management implementation
│   ├── MmapFile.cpp         # Cross-platform memory mapping implementation
│   ├── OrderBook.cpp        # Limit Order Book execution logic
│   └── Parser.cpp           # Parsing logic and iteration
└── CMakeLists.txt           # Root build configuration
```

## Implementation & Evolution (Devlogs)

### Phase 1: Baseline Architecture & The OS Shift
In the initial phase, I set up the core pipeline: a parser to decode binary messages, a `Market` class to manage tickers via a pre-allocated vector indexed by integer locate codes,
and an order book utilizing `std::unordered_map` for orders and `std::map` for price levels.

Initially benchmarking on Windows with standard `std::ifstream::read()`, the parser took over 1,200 seconds.
Moving the benchmark to Linux with `g++ -O3` dropped that down to ~410 seconds, providing a stable baseline and allowing me to use `perf` to inspect hardware metrics.

### Phase 2.1: Zero-Copy File I/O via Memory Mapping (`mmap`)
To eliminate the heavy kernel context switches and double-copying caused by `ifstream`, I implemented memory mapping (`mmap` on Linux, `MapViewOfFile` on Windows) wrapped in a clean cross-platform interface (`MmapFile`).

Treating the file as a continuous block of memory allowed me to ditch intermediate buffers entirely.
The parser now uses simple pointer arithmetic (`ptr += message_length`) and casts structs directly over the raw memory (`reinterpret_cast`). Endianness conversion from big-endian to little-endian leverages `std::byteswap` to generate optimal hardware instructions.

This dropped execution times drastically:
* **Windows:** ~1236s ➔ ~140s
* **Linux:** ~410s ➔ ~126s (with `perf` runtime around ~153s)

#### Hardware Profiling Insights
While memory mapping solved the I/O bottleneck, running Linux `perf` on Phase 2.1 revealed a new, deeper performance wall:
* **Low IPC (~0.19):** The CPU spent a massive amount of cycles stalled.
* **High Cache Miss Rate (~62.3%):** Out of ~10.6 billion references, about 6.6 billion missed the cache.

Eliminating the bulky, predictable memory-copying loops exposed the true bottleneck: the node-based standard containers (`std::map` and `std::unordered_map`). Every insertion or lookup triggered pointer chasing across random heap memory, starving the CPU while it waited on RAM fetches.

### Phase 2.2: Custom Data Structures and Hardware Optimization
To resolve the CPU stalls, I replaced the standard containers with custom contiguous data structures.

* **PriceLevelBook:** Replaced `std::map` with a custom order book backed by a continuous `std::vector`. Based on dataset profiling (90th percentile depth of 245 levels),
* the vector reserves 256 slots by default. This guarantees contiguous memory and zero reallocation overhead for the vast majority of the market, with insertions handled via binary search.
* **FlatHashMap:** Replaced `std::unordered_map` with a custom open-addressing hash map utilizing Robin Hood hashing. Robin Hood hashing (tracking Distance to Initial Bucket, or DIB) allows for a massive ~90% load factor while preventing clustering. This high load factor meant I only needed to pre-allocate ~170 million slots for the day's active orders,
* allowing the entire structure to fit cleanly into RAM and completely avoiding SSD memory swapping issues.

Replacing node-based heap allocations with contiguous arrays slashed the total instruction count by more than half and dropped execution times further:
* **Windows:** ~140s ➔ ~59s
* **Linux:** ~126s ➔ ~48.6s

## Tech Stack

* **Language:** C++20
* **Platforms:** Linux (GCC) & Windows (MSVC)
* **Build System:** CMake
* **Core Mechanisms:** Zero-copy memory mapping, packed structs, Robin Hood hashing, contiguous data structures, custom `#ifdef` OS abstractions, hardware profiling (`perf`).

## Building

```bash
# Configure the project in Release mode (cross-platform)
cmake -B build -DCMAKE_BUILD_TYPE=Release -S .

# Compile the engine
cmake --build build --config Release
```

## Running the Simulation

The engine accepts the path to the market data file as a command-line argument. You can provide any valid absolute or relative path to any of your ITCH-50 files.

**On Linux:**
```bash
cd build
./engine /path/to/your/market_data_file.bin
```

**On Windows:**
```cmd
cd build\Release
engine.exe C:\path\to\your\market_data_file.bin
```

### Fallback Default
If you run the executable without any arguments, it will safely fall back to checking a hardcoded relative path (`../data/08302019.NASDAQ_ITCH50`).

## Next Phases

Now that disk I/O is completely bypassed and the memory access bottlenecks are resolved with custom data structures, 
the next phase will focus on simulating a network stack. This will deviate slightly from the current local-file processing architecture, but it is necessary to accurately reflect how live trading systems receive and process UDP multicast packets in production.