#pragma once
#include <cstdint>
#include <unordered_map>
#include <map>
#include "messages.h"

struct ActiveOrder {
    uint64_t order_reference_number;
    char buy_sell_indicator;
    uint32_t shares;
    uint32_t price;
    uint16_t locate;
};

class OrderBook {
private:

    std::unordered_map<uint64_t, ActiveOrder> orders;


    std::map<uint32_t, uint64_t> bids;
    std::map<uint32_t, uint64_t> asks;

public:
    void process_add_order(const AddOrderMessage& msg);
};