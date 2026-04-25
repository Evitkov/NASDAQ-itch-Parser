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

public:
    Market();

    void process_stock_directory(const StockDirectoryMessage& msg);
    void process_add_order(const AddOrderMessage& msg);
};