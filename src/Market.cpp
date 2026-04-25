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
