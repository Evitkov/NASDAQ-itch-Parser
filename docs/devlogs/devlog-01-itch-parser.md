# Devlog #1: Parsing NASDAQ ITCH-50 & Building an Order Book (Phase 1)

## Introduction
This is the first phase of my NASDAQ ITCH-50 parser project. The goal of this phase isn't to write the most perfectly optimized, ultra-low-latency code right out of the gate. Instead, it’s about establishing a working baseline, understanding the domain, and learning by identifying bottlenecks and improving the code iteratively.

In this phase, I implemented the core architecture: a Parser to decode the binary messages, a Market class to manage the stocks, and an Order Book to maintain the bids, asks, and active orders for a single stock.

## What I Implemented

### 1. The Market Class
To manage the various stock directories efficiently, I avoided using string lookups for stock tickers whenever possible. Instead, the `Market` class uses a `std::vector` to hold stocks, indexed directly by their integer `locate` code.

Since we expect around 10,000 tickers (actually, for my specific historical file it was around 8,800), I used a `constexpr` to pre-allocate the vector size up front. This static sizing completely eliminates the overhead of resizing arrays and reallocating memory during runtime, which would otherwise hinder performance.

### 2. The Parser
The parser opens the ITCH binary file, reads the message type, calls the `fstream` read method, and loads the data directly into the message struct. Because NASDAQ files use big-endian formatting and my CPU is little-endian, I had to swap the byte order. Instead of writing manual bit-shifting logic, I utilized `std::byteswap`. This is a great modern C++ feature because the compiler translates it into a dedicated hardware instruction rather than actually doing the math in software, which achieves a small speedup.

### 3. The Order Book
The `OrderBook` contains the actual trading logic. It stores active orders in a hash map (`std::unordered_map`) by their order reference numbers, allowing O(1) lookups when orders are executed, canceled, or replaced.

For the bids and asks, I used `std::map`. This automatically sorts the prices using an underlying Red-Black tree structure, which is necessary since the ordering by price matters for the top of the book.

## The Environment Switch
I decided to benchmark my Phase 1 parser on Linux to improve performance, and more importantly, to reduce inconsistencies caused by frequent system interrupts on Windows. I used the `g++ -O3` flag to improve code performance, and since I decided to dual-boot, I could test the performance on the same system side-by-side.

The resulting speedup from the OS switch ended up being more drastic than I expected.
Code benchmarking results from start of parsing to end (using `std::chrono` printouts):
* **Windows:** ~1236 / 1217 seconds (2 runs)
* **Linux:** ~428 seconds (median over 5 runs)

Running the code natively on Linux stabilized the execution time and gave me more tools, like `perf`, to understand my system's bottlenecks.

Results from the perf tool running 5 times:

## Reflections & Next Steps
Because this is Phase 1, I made some design choices that I already know are inefficient, but to understand the optimizations, I will start here and improve the performance step by step. Here is what I plan to fix in Phase 2:

1. **I/O Bottleneck (`file.read`):** Currently, the parser reads each message into a struct using `std::ifstream::read()`. While relatively fast, making a system call to read small chunks of a file over and over is heavily bottlenecked by disk I/O. In the next phase, I will implement **Memory Mapping (mmap)** to load the file directly into the virtual address space, allowing the OS to handle paging and letting the CPU access the file as if it were an array in RAM.
2. **Red-Black Tree Overhead:** I used `std::map` for the bids and asks. While it keeps prices sorted, Red-Black trees are node-based. Every insertion or deletion requires pointer chasing, which is terrible for cache locality and results in frequent CPU cache misses. Moving forward, I want to replace this with a cache-friendly data structure, like a flat array or a custom B-Tree.