# ITCH Parser Architecture Notes

### The C++ Build Pipeline

* As a project scales from a single script to a complex system like an ITCH parser, dumping everything into one file becomes unmanageable and slows down compilation to a crawl. Understanding the C++ build pipeline is necessary to fix this.
* The preprocessor acts as a text replacement tool, pasting the contents of `#include "messages.h"` directly into the `.cpp` file. The compiler then translates each `.cpp` file blindly into an isolated object file (`.o`).
* Because the compiler works on these files independently, it has no idea that other files exist. This is the main advantage of the pipeline: if I modify a specific order-matching function, the compiler only has to rebuild that single object file. The Linker then stitches the addresses of all the binaries together into the final executable.
* If we didn't split the logic into headers and source files, the compiler would have to re-translate the entire codebase every time we changed a single line of code, making iterative testing impossible.

### POD Structs and Memory Packing

* To process NASDAQ network packets efficiently, we need to map raw incoming bytes directly into memory. We use `struct` instead of `class` because a struct defaults to public access, letting us read the data directly without writing useless getter/setter boilerplate.
* More importantly, the data structure must be a POD (Plain Old Data) type. If we used standard C++ classes with virtual functions, constructors, or inheritance, the compiler would inject hidden metadata (like vtables) into the object. This would destroy the 1:1 mapping with the raw network bytes, causing our parser to read garbage memory.
* We also have to handle memory alignment. By default, compilers insert empty "padding" bytes between variables because CPUs read memory faster in aligned chunks. However, NASDAQ tightly packs its ITCH messages without padding to save network bandwidth.
* If we simply cast the raw network bytes into our struct, the compiler’s hidden padding would offset the memory addresses. A 6-byte timestamp would misalign the rest of the struct, pulling in the wrong bytes for the fields that follow.
* To prevent this, we wrap our structs in `#pragma pack(push, 1)`. This forces the compiler to drop its CPU alignment rules and map the exact, unpadded byte layout of the network message. We then pop it back so the rest of the program remains optimized. We sacrifice a tiny bit of CPU fetch speed to ensure the data is actually read correctly.

### The Problem with Syscalls and Interrupts

* The entire point of a trading engine is deterministic low latency. If we naively used standard `std::cout` prints or basic file read loops for every message, the program would constantly trigger System Calls (syscalls).
* A syscall forces a context switch. The CPU has to drop out of user-space execution, flush its highly optimized L1/L2 caches, save its registers, and hand control over to the OS kernel. This completely halts the instruction pipeline.
* Similarly, standard I/O relies on hardware interrupts. When the disk fetches data, it fires an electrical signal that forces the CPU to drop whatever it is doing.
* In a high-frequency context where nanoseconds matter, waiting on the OS is a massive bottleneck. A proper architecture avoids this by keeping the "hot path" (the actual order parsing and matching) entirely in user-space, doing things like mapping the file directly into memory or pushing logging tasks to a separate background thread.

### Data Types and Cross-Platform Quirks

* For things like Order IDs and stock quantities, we strictly use fixed-width integer types like `uint64_t` instead of standard types like `long long`.
* The C++ standard only guarantees that `long long` is *at least* 64 bits. If we ran this parser on a different architecture or compiler where `long long` defaults to 128 bits, our memory casts would immediately break, reading out of bounds and corrupting the data. `uint64_t` guarantees exactly 64 bits regardless of the system.
* Additionally, Order IDs and shares cannot be negative. By using unsigned types, we drop the sign bit. This allows the variable to use all 64 bits for the number itself, doubling the maximum trackable value and preventing integer overflow bugs when handling massive daily trading volumes.