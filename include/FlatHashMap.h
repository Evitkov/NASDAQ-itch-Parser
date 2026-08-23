#pragma once

#include <vector>
#include <cstdint>
#include "messages.h"

class FlatHashMap {
public:
    explicit FlatHashMap(size_t max_capacity);

    void insert(ActiveOrder value);
    ActiveOrder* find(uint64_t key);
    void erase(uint64_t key);

private:
    std::vector<ActiveOrder> slots;
    size_t capacity;
};