# Trading Engine: Core Parser & Memory-Mapped Simulation

## Motivation

I built this project to dive deep into C++ systems programming and low-level performance optimization. Instead of tackling complex networking right away, I wanted to focus on the core mechanics of high-throughput data processing by building a NASDAQ ITCH-50 parser and a Limit Order Book (LOB) from scratch. It is a practical playground for exploring memory layouts, data structure trade-offs, and hardware bottlenecks without relying on heavy external frameworks.

## Architecture

The engine runs offline as a deterministic simulation, processing historical market data from a local binary file. It is built to be cross-platform, using OS-specific system calls for both Windows and Linux to handle heavy I/O efficiently.

```text
Engine/
├── data/                    # Market data files (e.g., ITCH-50 .bin files)
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
```

## Implementation & Evolution (Devlogs)

### Phase 1: Baseline Architecture & The OS Shift
In the initial phase, I set up the core pipeline: a parser to decode binary messages, a `Market` class to manage tickers via a pre-allocated vector indexed by integer locate codes, and an order book utilizing `std::unordered_map` for orders and `std::map` for price levels.

Initially benchmarking on Windows with standard `std::ifstream::read()`, the parser took over 1,200 seconds. Moving the benchmark to Linux with `g++ -O3` dropped that down to ~410 seconds, providing a stable baseline and allowing me to use `perf` to inspect hardware metrics.

### Phase 2: Zero-Copy File I/O via Memory Mapping (`mmap`)
To eliminate the heavy kernel context switches and double-copying caused by `ifstream`, I implemented memory mapping (`mmap` on Linux, `MapViewOfFile` on Windows) wrapped in a clean cross-platform interface (`MmapFile`).

Treating the file as a continuous block of memory allowed me to ditch intermediate buffers entirely. The parser now uses simple pointer arithmetic (`ptr += message_length`) and casts structs directly over the raw memory (`reinterpret_cast`). Endianness conversion from big-endian to little-endian leverages `std::byteswap` to generate optimal hardware instructions.

This dropped execution times drastically:
* **Windows:** ~1236s ➔ ~140s
* **Linux:** ~410s ➔ ~126s (with `perf` runtime around ~153s)

### Hardware Profiling Insights
While memory mapping solved the I/O bottleneck, running Linux `perf` on Phase 2 revealed a new, deeper performance wall:
* **Low IPC (~0.19):** The CPU spends a massive amount of cycles stalled.
* **High Cache Miss Rate (~62.3%):** Out of ~10.6 billion references, about 6.6 billion missed the cache.

The drop in IPC and spike in cache misses aren't regressions; rather, eliminating the bulky, predictable memory-copying loops of Phase 1 exposed the true bottleneck: the node-based standard containers (`std::map` and `std::unordered_map`). Every insertion or lookup triggers pointer chasing across random heap memory, starving the CPU while it waits on RAM fetches.

## Tech Stack

* **Language:** C++20
* **Platforms:** Linux (GCC) & Windows (MSVC)
* **Build System:** CMake
* **Core Mechanisms:** Zero-copy memory mapping, packed structs, custom `#ifdef` OS abstractions, hardware profiling (`perf`).

## Building

```bash
# Configure the project in Release mode (cross-platform)
cmake -B build -DCMAKE_BUILD_TYPE=Release -S .

# Compile the engine
cmake --build build --config Release
```

## Running the Simulation

The input data file path is currently hardcoded in `src/main.cpp`:

```cpp
std::string filepath = "../data/08302019.NASDAQ_ITCH50";
```

This is a **relative path resolved against the working directory the binary is launched from**, so:

* Place your ITCH-50 data file at `data/08302019.NASDAQ_ITCH50` in the project root.
* Run the executable from inside `build/` (or your IDE's build/run output directory) so that `../data/` correctly resolves up to the project root.
* If you use a different data file, update the `filepath` string in `main.cpp` accordingly and rebuild.

**On Linux:**
```bash
cd build
./engine
```

**On Windows:**
```cmd
cd build\Release
engine.exe
```

**Running from an IDE:** set the run configuration's working directory to the `build` (or `build/Release`) folder — not the project root — otherwise the hardcoded `../data/...` path will fail to resolve.

## Next Phases

Now that disk I/O is completely bypassed and memory mapping is stable, the next phase will focus entirely on **Cache Optimization**. I plan to replace the node-based standard library containers with cache-friendly, contiguous data structures.