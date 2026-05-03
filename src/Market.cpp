#include <cstdint>
#include "Market.h"


// we resize the arrays to 10000 - around what amount of tickers we expect so we save resizing performance
// we use static - one for class constexpr - not even in ram but CPU instr so we save time
Market::Market() {
    static constexpr uint16_t size = 10000;
    tickers.resize(size);
    books.resize(size);

}
void Market::process_add_order(const AddOrderMessage& msg) {
    books[msg.locate].process_add_order(msg);
}
void Market::process_stock_directory(const StockDirectoryMessage& msg) {
    tickers[msg.locate] = msg.stock;
    ticker_to_locate[msg.stock] = msg.locate;
}
void Market::process_cancel_order(const OrderCancelMessage& msg) {
    books[msg.locate].process_cancel_order(msg);
}

void Market::process_execute_order(const OrderExecutedMessage& msg) {
    books[msg.locate].process_execute_order(msg);
    total_volume += msg.executed_shares;
}

void Market::process_execute_with_price(const OrderExecutedWithPriceMessage& msg) {
    books[msg.locate].process_execute_with_price(msg);
    total_volume += msg.executed_shares;
}

void Market::process_delete_order(const OrderDeleteMessage& msg) {
    books[msg.locate].process_delete_order(msg);
}

void Market::process_replace_order(const OrderReplaceMessage& msg) {
    books[msg.locate].process_replace_order(msg);
}
void Market::process_trade(const TradeMessage& msg) {
    total_volume += msg.shares;

}
void Market::process_cross_trade(const CrossTradeMessage& msg) {
    total_volume += msg.shares;

}