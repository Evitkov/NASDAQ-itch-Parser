# Trading Engine: Stage 1 (Order Book & File Simulation)

## Motivation

I built this project to deepen my practical understanding of C++ and software architecture by exploring a domain I find interesting and very connected to low level concepts: trading systems. Instead of getting bogged down in complex network programming right away, I wanted to start with the core mechanics. This first stage focuses entirely on efficiently parsing market data and building a high-performance Limit Order Book (LOB) from the ground up. It has been a hands-on way to learn about memory layout, optimal data structure selection, and low-level performance tuning without relying on heavy external frameworks.

## Architecture

This initial stage operates offline within a Windows environment. Rather than ingesting live network packets, the engine reads historical market data directly from a local binary file, acting as a deterministic simulation of an exchange feed.

```text
Engine/
├── engine/                  # Core application logic
│   ├── main.cpp             # Application entry point and file simulation loop
│   ├── Parser.hpp           # Market data file parser and deserializer
│   ├── Order_Book.cpp/hpp   # Limit Order Book (LOB) implementation
├── common/                  # Shared data structures and definitions
│   ├── Messages.hpp         # Packed message structs matching the data format
│   └── Types.hpp            # Order and PriceLevel cache-friendly layouts
├── Docs/                    # Project documentation
│   ├── devlogs/             # Formal architectural decision records
│   └── field_notes/         # Unfiltered implementation insights and learnings
└── CMakeLists.txt           # Root build configuration
```

## The Interesting Parts

### Zero-Allocation Parsing

Even though we are reading from a file instead of a network socket, the parsing philosophy remains zero-copy where possible. Large chunks of the binary market data file are read into a memory buffer. Instead of deserializing these bytes into new objects, I overlay tightly packed C++ structs directly onto the buffer using `reinterpret_cast`. By enforcing strict memory layouts with compiler directives (like `#pragma pack`), the CPU knows exactly how to read the fields at a given address. This completely eliminates dynamic allocation and `memcpy` overhead during the critical parsing loop.

### The Limit Order Book (LOB) Architecture

The heart of the simulation is the Order Book. It needs to reflect the current state of the market with extreme efficiency. The internal structures are designed to keep the "hot" operations as fast as possible:
* **Price Levels:** Maintained in a Red-Black Tree (`std::map`). Because price levels naturally need to be sorted (e.g., to quickly find the best bid or ask), the tree structure guarantees the top of the book is always immediately accessible.
* **Order Tracking:** Stored in a `std::list` to maintain strict FIFO time priority.
* **O(1) Lookups:** To handle rapid order modifications and cancellations, the engine maintains a `std::unordered_map` linking an Order ID directly to its memory location. This means a cancel message triggers a direct pointer dereference rather than a costly search through the book.

### Deterministic File Simulation

By driving the engine purely from a binary file rather than a live network interface, the system becomes entirely deterministic. Time in the engine is not governed by the system clock, but by the timestamps stamped on the parsed messages. This allows for highly reproducible profiling and debugging on Windows, letting me measure the exact CPU cycles required for specific order book operations without network jitter skewing the results.

## Tech Stack

* **Language:** C++20
* **Platform:** Windows (MSVC)
* **Build System:** CMake
* **Core Mechanisms:** Binary file I/O, packed structs, customized standard library containers.

## Building

The project is configured to be built easily on Windows using CMake and a compatible compiler like MSVC.

```cmd
# Create the build directory and configure the project
cmake -B build -S .

# Compile the project in Release mode for optimal performance
cmake --build build --config Release
```

## Running and Simulation

To run the simulation, the engine requires a binary market data file. The application reads this file sequentially, processing each message to reconstruct and update the Limit Order Book in memory.

```cmd
# Run the compiled executable, passing the path to the data file
.\build\Release\engine.exe path\to\market_data.bin
```

As the simulation processes the file, the engine outputs periodic telemetry regarding parsing throughput (messages per second) and the current depth of the order book.

## Documentation

The `Docs/` directory contains all project documentation, split into two distinct categories:
* **devlogs/:** Formal records of major architectural decisions. This includes the reasoning behind specific data structure choices for the LOB and structural design patterns.
* **field_notes/:** Informal scratchpads, learnings, and observations made during development. This covers nuances of C++20, file I/O performance traits on Windows, and raw benchmarking thoughts.