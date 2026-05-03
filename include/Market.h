#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "messages.h"
#include "OrderBook.h"

class Market {
private:
    // foward dictionary - since we store in array based on locate, we need this in the end for output to map locate to ticker
    std::vector<std::string> tickers;

    // backwards dictionary - could be used for ex for cli to see orders for ticker ...
    std::unordered_map<std::string, uint16_t> ticker_to_locate;

    //We use array for locating orderbook for each stock by locate - fast lookups and no hashing math needed
    std::vector<OrderBook> books;

    //For correctness checks
    uint64_t total_volume = 0;

public:
    Market();

    // method to check total volume of all shares executed for debugging
    uint64_t get_total_volume() const { return total_volume; }

    void process_stock_directory(const StockDirectoryMessage& msg);
    void process_add_order(const AddOrderMessage& msg);

    void process_cancel_order(const OrderCancelMessage& msg) ;

    void process_execute_order(const OrderExecutedMessage& msg) ;

    void process_execute_with_price(const OrderExecutedWithPriceMessage& msg) ;

    void process_trade(const TradeMessage& msg) ;

    void process_cross_trade(const CrossTradeMessage& msg) ;

    void process_delete_order(const OrderDeleteMessage& msg) ;

    void process_replace_order(const OrderReplaceMessage& msg) ;
};