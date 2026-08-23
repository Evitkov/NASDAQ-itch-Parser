#pragma once
#include <cstdint>

// Converts 6-byte timestamp array into a 64-bit integer.
// Uses inline to eliminate function-call overhead and prevent Linker ODR violations.
// This happens because inline means the compiler copies only the code inside the function there. No function calls - faster.
// Since without inline the whole function would be pasted - possible ODR if included in more files
inline uint64_t parse_6byte_timestamp(const uint8_t ts[6]) {
    return (static_cast<uint64_t>(ts[0]) << 40) |
           (static_cast<uint64_t>(ts[1]) << 32) |
           (static_cast<uint64_t>(ts[2]) << 24) |
           (static_cast<uint64_t>(ts[3]) << 16) |
           (static_cast<uint64_t>(ts[4]) << 8)  |
           static_cast<uint64_t>(ts[5]);
}
//splitmix64 hash function used for 64bit ints
inline uint64_t splitmix64_hash(uint64_t key) {
    uint64_t x = key;
    x ^= x >> 30;
    x *= 0xbf58476d1ce4e5b9ULL;
    x ^= x >> 27;
    x *= 0x94d049bb133111ebULL;
    x ^= x >> 31;
    return x;
}