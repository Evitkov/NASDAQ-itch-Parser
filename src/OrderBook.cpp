#include "OrderBook.h"

void OrderBook::process_add_order(const AddOrderMessage &msg) {
    ActiveOrder order;
    order.order_reference_number = msg.order_reference_number;
    order.price = msg.price;
    order.buy_sell_indicator = msg.buy_sell_indicator;
    order.shares=msg.shares;
    order.locate=msg.locate;

    //save to the hashmap
    orders[msg.order_reference_number] = order;

    //update the redblack trees with asks and bids
    if (msg.buy_sell_indicator=='B') {
        bids[msg.price]+=msg.shares;
    }
    else {
        asks[msg.price]+=msg.shares;
    }

}
