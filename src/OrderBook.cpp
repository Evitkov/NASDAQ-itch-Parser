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
void OrderBook::process_cancel_order(const OrderCancelMessage& msg) {
    ActiveOrder& order = orders[msg.order_reference_number];

  //Update amount for bids and asks
    if (order.buy_sell_indicator == 'B') {
        bids[order.price] -= msg.canceled_shares;

        if (bids[order.price] == 0) {
            bids.erase(order.price);
        }
    }
    else if (order.buy_sell_indicator == 'S') {
        asks[order.price] -= msg.canceled_shares;

        if (asks[order.price] == 0) {
            asks.erase(order.price);
        }
    }

    // Update the orders hash table
    order.shares -= msg.canceled_shares;

    if (order.shares == 0) {
        orders.erase(msg.order_reference_number);
    }
}
void OrderBook::process_execute_order(const OrderExecutedMessage& msg) {
    ActiveOrder& order = orders[msg.order_reference_number];

    //Update amount for bids and asks
    if (order.buy_sell_indicator == 'B') {
        bids[order.price] -= msg.executed_shares;

        if (bids[order.price] == 0) {
            bids.erase(order.price);
        }
    }
    else if (order.buy_sell_indicator == 'S') {
        asks[order.price] -= msg.executed_shares;

        if (asks[order.price] == 0) {
            asks.erase(order.price);
        }
    }

    // Update the orders hash table
    order.shares -= msg.executed_shares;

    if (order.shares == 0) {
        orders.erase(msg.order_reference_number);
    }
}
void OrderBook::process_execute_with_price(const OrderExecutedWithPriceMessage& msg) {
    ActiveOrder& order = orders[msg.order_reference_number];

    //Update amount for bids and asks
    if (order.buy_sell_indicator == 'B') {
        bids[order.price] -= msg.executed_shares;

        if (bids[order.price] == 0) {
            bids.erase(order.price);
        }
    }
    else if (order.buy_sell_indicator == 'S') {
        asks[order.price] -= msg.executed_shares;

        if (asks[order.price] == 0) {
            asks.erase(order.price);
        }
    }

    // Update the orders hash table
    order.shares -= msg.executed_shares;

    if (order.shares == 0) {
        orders.erase(msg.order_reference_number);
    }

}

void OrderBook::process_delete_order(const OrderDeleteMessage& msg) {
    ActiveOrder& order = orders[msg.order_reference_number];

    //Update amount for bids and asks
    if (order.buy_sell_indicator == 'B') {
        bids[order.price] -= order.shares;

        if (bids[order.price] == 0) {
            bids.erase(order.price);
        }
    }
    else if (order.buy_sell_indicator == 'S') {
        asks[order.price] -= order.shares;

        if (asks[order.price] == 0) {
            asks.erase(order.price);
        }
    }
    orders.erase(msg.order_reference_number);


}

void OrderBook::process_replace_order(const OrderReplaceMessage& msg) {
    ActiveOrder& order = orders[msg.original_order_reference_number];
    const char buy_sell  = order.buy_sell_indicator;

    //Update amount for bids and asks
    if (order.buy_sell_indicator == 'B') {
        bids[order.price] -= order.shares;

        if (bids[order.price] == 0) {
            bids.erase(order.price);
        }
    }
    else if (order.buy_sell_indicator == 'S') {
        asks[order.price] -= order.shares;

        if (asks[order.price] == 0) {
            asks.erase(order.price);
        }
    }

    orders.erase(msg.original_order_reference_number);

    ActiveOrder new_order;
    new_order.order_reference_number = msg.new_order_reference_number;
    new_order.price = msg.price;
    new_order.buy_sell_indicator = buy_sell;
    new_order.shares=msg.shares;
    new_order.locate=msg.locate;

    //save to the hashmap
    orders[msg.new_order_reference_number] = new_order;

    //update the redblack trees with asks and bids
    if (new_order.buy_sell_indicator=='B') {
        bids[msg.price]+=msg.shares;
    }
    else {
        asks[msg.price]+=msg.shares;
    }
}

