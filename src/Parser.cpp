#include "Parser.h"
#include "MmapFile.h"
#include <bit>
#include <iostream>
#include <string>

#include "messages.h"
#include "utils.h"

void Parser::parse() {
    MmapFile mapped_file = MmapFile(filepath);
    const char* current = mapped_file.get_start();
    if (current==nullptr) {
        std::cerr << "[ERROR] Opening the file failed" << std::endl;
        return;
    }
    const char* end = current+mapped_file.get_length();

    std::cout << "[SYSTEM] File opened successfully" << std::endl;

    message_count = 0;
    //Read the 2-byte length header first - specific to NASDAQ historical files

    while (current<end) {
        message_count++;
        if (message_count % 10000000 == 0) {
            std::cout << "[PROGRESS] Processed " << (message_count / 1000000) << " million messages..." << std::endl;
        }
        uint16_t message_length = *reinterpret_cast<const uint16_t*>(current);
        // The length is big endian and our CPU is little endian we to use the byteswap function - also described later.
        message_length = std::byteswap(message_length);

        //Message type
        char message_type;
        message_type = current[2];

        switch (message_type) {
            case 'S': {

                // in c++ we can step back "seek -1" from our current position

                SystemEventMessage msg = *reinterpret_cast<const SystemEventMessage*>(current + 2);


                //We use std::byteswap instead of manual bit-shifting.
                // The compiler translates this into a compiler intrinsic. This means CPU doesn't have to do math and
                // this triggers a dedicated hardware instruction
                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);


                std::cout << "[SYSTEM EVENT] "
                        <<"Code: " << msg.event_code
                       << " | Locate: " << msg.locate
                       << " | Tracking: " << msg.tracking_number
                       << std::endl;


                break;

            }
            case 'R': {

                StockDirectoryMessage msg = *reinterpret_cast<const StockDirectoryMessage*>(current + 2);


                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.round_lot_size=std::byteswap(msg.round_lot_size);
                msg.etp_leverage_factor=std::byteswap(msg.etp_leverage_factor);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);

                market.process_stock_directory(msg);
                break;

            }
            case 'H': {
                StockTradingActionMessage msg = *reinterpret_cast<const StockTradingActionMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'Y': {
                RegShoRestrictionMessage msg = *reinterpret_cast<const RegShoRestrictionMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'L': {
                MarketParticipantPositionMessage msg = *reinterpret_cast<const MarketParticipantPositionMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'V': {
                MwcbDeclineLevelMessage msg = *reinterpret_cast<const MwcbDeclineLevelMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.level_1 = std::byteswap(msg.level_1);
                msg.level_2 = std::byteswap(msg.level_2);
                msg.level_3 = std::byteswap(msg.level_3);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'W': {
                MwcbStatusMessage msg = *reinterpret_cast<const MwcbStatusMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'K': {
                IpoQuotingPeriodUpdateMessage msg = *reinterpret_cast<const IpoQuotingPeriodUpdateMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.ipo_quotation_release_time = std::byteswap(msg.ipo_quotation_release_time);
                msg.ipo_price = std::byteswap(msg.ipo_price);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'J': {
                LuldAuctionCollarMessage msg = *reinterpret_cast<const LuldAuctionCollarMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.reference_price = std::byteswap(msg.reference_price);
                msg.upper_collar = std::byteswap(msg.upper_collar);
                msg.lower_collar = std::byteswap(msg.lower_collar);
                msg.auction_collar_extension = std::byteswap(msg.auction_collar_extension);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'h': {
                OperationalHaltMessage msg = *reinterpret_cast<const OperationalHaltMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'A': {
                AddOrderMessage msg = *reinterpret_cast<const AddOrderMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.order_reference_number = std::byteswap(msg.order_reference_number);
                msg.shares = std::byteswap(msg.shares);
                msg.price = std::byteswap(msg.price);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);

                market.process_add_order(msg);
                break;
            }
            case 'F': {
                AddOrderMpidMessage msg = *reinterpret_cast<const AddOrderMpidMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.order_reference_number = std::byteswap(msg.order_reference_number);
                msg.shares = std::byteswap(msg.shares);
                msg.price = std::byteswap(msg.price);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'E': {
                OrderExecutedMessage msg = *reinterpret_cast<const OrderExecutedMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.order_reference_number = std::byteswap(msg.order_reference_number);
                msg.executed_shares = std::byteswap(msg.executed_shares);
                msg.match_number = std::byteswap(msg.match_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);

                market.process_execute_order(msg);
                break;
            }
            case 'C': {
                OrderExecutedWithPriceMessage msg = *reinterpret_cast<const OrderExecutedWithPriceMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.order_reference_number = std::byteswap(msg.order_reference_number);
                msg.executed_shares = std::byteswap(msg.executed_shares);
                msg.match_number = std::byteswap(msg.match_number);
                msg.execution_price = std::byteswap(msg.execution_price);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'X': {
                OrderCancelMessage msg = *reinterpret_cast<const OrderCancelMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.order_reference_number = std::byteswap(msg.order_reference_number);
                msg.canceled_shares = std::byteswap(msg.canceled_shares);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);

                market.process_cancel_order(msg);
                break;
            }
            case 'D': {
                OrderDeleteMessage msg = *reinterpret_cast<const OrderDeleteMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.order_reference_number = std::byteswap(msg.order_reference_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'U': {
                OrderReplaceMessage msg = *reinterpret_cast<const OrderReplaceMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.original_order_reference_number = std::byteswap(msg.original_order_reference_number);
                msg.new_order_reference_number = std::byteswap(msg.new_order_reference_number);
                msg.shares = std::byteswap(msg.shares);
                msg.price = std::byteswap(msg.price);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'P': {
                TradeMessage msg = *reinterpret_cast<const TradeMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.order_reference_number = std::byteswap(msg.order_reference_number);
                msg.shares = std::byteswap(msg.shares);
                msg.price = std::byteswap(msg.price);
                msg.match_number = std::byteswap(msg.match_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'Q': {
                CrossTradeMessage msg = *reinterpret_cast<const CrossTradeMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.shares = std::byteswap(msg.shares);
                msg.cross_price = std::byteswap(msg.cross_price);
                msg.match_number = std::byteswap(msg.match_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'B': {
                BrokenTradeMessage msg = *reinterpret_cast<const BrokenTradeMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.match_number = std::byteswap(msg.match_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'I': {
                NoiiMessage msg = *reinterpret_cast<const NoiiMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.paired_shares = std::byteswap(msg.paired_shares);
                msg.imbalance_shares = std::byteswap(msg.imbalance_shares);
                msg.far_price = std::byteswap(msg.far_price);
                msg.near_price = std::byteswap(msg.near_price);
                msg.current_reference_price = std::byteswap(msg.current_reference_price);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'O': {
                DlcrMessage msg = *reinterpret_cast<const DlcrMessage*>(current + 2);

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.minimum_allowable_price = std::byteswap(msg.minimum_allowable_price);
                msg.maximum_allowable_price = std::byteswap(msg.maximum_allowable_price);
                msg.near_execution_price = std::byteswap(msg.near_execution_price);
                msg.near_execution_time = std::byteswap(msg.near_execution_time);
                msg.lower_price_range_collar = std::byteswap(msg.lower_price_range_collar);
                msg.upper_price_range_collar = std::byteswap(msg.upper_price_range_collar);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }

            default: {
                break;
            }

        }
        current += 2+message_length;

    }

}



uint64_t Parser::get_message_count() {
    return message_count;
}