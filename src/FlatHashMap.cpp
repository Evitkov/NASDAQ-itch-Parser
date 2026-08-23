#include "FlatHashMap.h"
#include "utils.h"
#include "messages.h"
#include <utility>

FlatHashMap::FlatHashMap(size_t max_capacity) {
    capacity = max_capacity;
    slots.resize(capacity);
}

void FlatHashMap::insert(ActiveOrder order) {
    size_t pos = splitmix64_hash(order.order_reference_number) % capacity;
    order.dib = 0;
    order.occupied = true;
    while (true) {
        if (!slots[pos].occupied) {
            slots[pos] = order;
            return;
        }
        if (slots[pos].order_reference_number == order.order_reference_number) {
            slots[pos] = order;
            return;
        }
        if (order.dib > slots[pos].dib) {
            std::swap(order, slots[pos]);
        }

        pos++;
       pos = pos % capacity;
        order.dib++;
    }
}

ActiveOrder* FlatHashMap::find(uint64_t key) {
    size_t pos = splitmix64_hash(key) % capacity;
    uint16_t current_dib = 0;

    while (slots[pos].occupied) {
        if (slots[pos].order_reference_number == key) {
            return &slots[pos];
        }
        if (current_dib > slots[pos].dib) {
            return nullptr;
        }

        pos++;
       pos=pos % capacity;
        current_dib++;
    }
    return nullptr;
}

void FlatHashMap::erase(uint64_t key) {
    size_t pos = splitmix64_hash(key) % capacity;
    uint16_t current_dib = 0;

    while (slots[pos].occupied) {
        if (slots[pos].order_reference_number == key) {
            size_t cur= pos;
            size_t next = (cur+1 >= capacity) ? 0 : cur + 1;

            while (slots[next].occupied && slots[next].dib > 0) {
                slots[cur] = slots[next];
                slots[cur].dib--;
                cur = next;
                next = (next +1 >= capacity) ? 0 : next + 1;
            }
            slots[cur].occupied = false;
            slots[cur].dib = 0;
            return;
        }
        if (current_dib > slots[pos].dib) {
            return;
        }
        pos++;
        pos = pos%capacity;
        current_dib++;
    }
}