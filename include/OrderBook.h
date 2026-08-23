#pragma once

#include <cstdint>
#include "messages.h"
#include "FlatHashMap.h"
#include "PriceLevelBook.h"

class OrderBook {
public:
    inline static uint64_t current_orders = 0;
    inline static uint64_t peak_orders = 0;

private:
    inline static FlatHashMap orders{170000000};

    PriceLevelBook bids;
    PriceLevelBook asks;

public:
    void process_add_order(const AddOrderMessage& msg);
    void process_cancel_order(const OrderCancelMessage& msg);
    void process_execute_order(const OrderExecutedMessage& msg);
    void process_execute_with_price(const OrderExecutedWithPriceMessage& msg);
    void process_delete_order(const OrderDeleteMessage& msg);
    void process_replace_order(const OrderReplaceMessage& msg);
};