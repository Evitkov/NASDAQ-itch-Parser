#include "OrderBook.h"

void OrderBook::process_add_order(const AddOrderMessage &msg) {
    ActiveOrder order;
    order.order_reference_number = msg.order_reference_number;
    order.price = msg.price;
    order.buy_sell_indicator = msg.buy_sell_indicator;
    order.shares = msg.shares;
    order.locate = msg.locate;

    orders.insert(order);

    current_orders++;
    if (current_orders > peak_orders) {
        peak_orders = current_orders;
    }

    if (msg.buy_sell_indicator == 'B') {
        bids.add(msg.price, msg.shares);
    } else {
        asks.add(msg.price, msg.shares);
    }
}

void OrderBook::process_cancel_order(const OrderCancelMessage& msg) {
    ActiveOrder* order_ptr = orders.find(msg.order_reference_number);
    if (!order_ptr) return;
    ActiveOrder& order = *order_ptr;

    if (order.buy_sell_indicator == 'B') {
        bids.remove(order.price, msg.canceled_shares);
    } else if (order.buy_sell_indicator == 'S') {
        asks.remove(order.price, msg.canceled_shares);
    }

    order.shares -= msg.canceled_shares;
    if (order.shares == 0) {
        orders.erase(msg.order_reference_number);
        current_orders--;
    }
}

void OrderBook::process_execute_order(const OrderExecutedMessage& msg) {
    ActiveOrder* order_ptr = orders.find(msg.order_reference_number);
    if (!order_ptr) return;
    ActiveOrder& order = *order_ptr;

    if (order.buy_sell_indicator == 'B') {
        bids.remove(order.price, msg.executed_shares);
    } else if (order.buy_sell_indicator == 'S') {
        asks.remove(order.price, msg.executed_shares);
    }

    order.shares -= msg.executed_shares;
    if (order.shares == 0) {
        orders.erase(msg.order_reference_number);
        current_orders--;
    }
}

void OrderBook::process_execute_with_price(const OrderExecutedWithPriceMessage& msg) {
    ActiveOrder* order_ptr = orders.find(msg.order_reference_number);
    if (!order_ptr) return;
    ActiveOrder& order = *order_ptr;

    if (order.buy_sell_indicator == 'B') {
        bids.remove(order.price, msg.executed_shares);
    } else if (order.buy_sell_indicator == 'S') {
        asks.remove(order.price, msg.executed_shares);
    }

    order.shares -= msg.executed_shares;
    if (order.shares == 0) {
        orders.erase(msg.order_reference_number);
        current_orders--;
    }
}

void OrderBook::process_delete_order(const OrderDeleteMessage& msg) {
    ActiveOrder* order_ptr = orders.find(msg.order_reference_number);
    if (!order_ptr) return;
    ActiveOrder& order = *order_ptr;

    if (order.buy_sell_indicator == 'B') {
        bids.remove(order.price, order.shares);
    } else if (order.buy_sell_indicator == 'S') {
        asks.remove(order.price, order.shares);
    }

    orders.erase(msg.order_reference_number);
    current_orders--;
}

void OrderBook::process_replace_order(const OrderReplaceMessage& msg) {
    ActiveOrder* order_ptr = orders.find(msg.original_order_reference_number);
    if (!order_ptr) return;
    ActiveOrder& order = *order_ptr;

    const char buy_sell = order.buy_sell_indicator;

    if (order.buy_sell_indicator == 'B') {
        bids.remove(order.price, order.shares);
    } else if (order.buy_sell_indicator == 'S') {
        asks.remove(order.price, order.shares);
    }

    orders.erase(msg.original_order_reference_number);

    ActiveOrder new_order;
    new_order.order_reference_number = msg.new_order_reference_number;
    new_order.price = msg.price;
    new_order.buy_sell_indicator = buy_sell;
    new_order.shares = msg.shares;
    new_order.locate = msg.locate;

    orders.insert(new_order);

    if (new_order.buy_sell_indicator == 'B') {
        bids.add(msg.price, msg.shares);
    } else {
        asks.add(msg.price, msg.shares);
    }
}