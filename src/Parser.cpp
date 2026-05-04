#include "Parser.h"
#include <bit>
#include <fstream>
#include <iostream>
#include <string>

#include "messages.h"
#include "utils.h"

void Parser::parse() {
     //establish connection to the binary file - we can then use method read, this will take current byte/bytes load it into RAM.
    std::ifstream file(filepath,std::ios::binary);

    //check if connection was established correctly
    if (!file.is_open()) {
        std::cerr << "[ERROR] The provided path is incorrect: " << filepath << std::endl;
        return ;
    }


    std::cout << "[SYSTEM] File opened successfully" << std::endl;

    message_count = 0;
    //Read the 2-byte length header first - specific to NASDAQ historical files
    uint16_t message_length;
    while (file.read(reinterpret_cast<char*>(&message_length),2)) {
        message_count++;
        if (message_count % 10000000 == 0) {
            std::cout << "[PROGRESS] Processed " << (message_count / 1000000) << " million messages..." << std::endl;
        }

        // The length is big endian and our CPU is little endian we to use the byteswap function - also described later.
        message_length = std::byteswap(message_length);

        //Message type
        char message_type;
        file.read(&message_type, 1);

        switch (message_type) {
            case 'S': {

                // in c++ we can step back "seek -1" from our current position
                file.seekg(-1, std::ios::cur);

                SystemEventMessage msg;

                // Take the address of message, and pretend it's just a pointer to raw chars not our struct
                file.read(reinterpret_cast<char*>(&msg), sizeof(SystemEventMessage));

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

                file.seekg(-1, std::ios::cur);

                StockDirectoryMessage msg;

                file.read(reinterpret_cast<char*>(&msg), sizeof(StockDirectoryMessage));


                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.round_lot_size=std::byteswap(msg.round_lot_size);
                msg.etp_leverage_factor=std::byteswap(msg.etp_leverage_factor);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);

                market.process_stock_directory(msg);
                break;

            }
            case 'H': {
                file.seekg(-1, std::ios::cur);
                StockTradingActionMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(StockTradingActionMessage));

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'Y': {
                file.seekg(-1, std::ios::cur);
                RegShoRestrictionMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(RegShoRestrictionMessage));

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'L': {
                file.seekg(-1, std::ios::cur);
                MarketParticipantPositionMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(MarketParticipantPositionMessage));

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'V': {
                file.seekg(-1, std::ios::cur);
                MwcbDeclineLevelMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(MwcbDeclineLevelMessage));

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.level_1 = std::byteswap(msg.level_1);
                msg.level_2 = std::byteswap(msg.level_2);
                msg.level_3 = std::byteswap(msg.level_3);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'W': {
                file.seekg(-1, std::ios::cur);
                MwcbStatusMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(MwcbStatusMessage));

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'K': {
                file.seekg(-1, std::ios::cur);
                IpoQuotingPeriodUpdateMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(IpoQuotingPeriodUpdateMessage));

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.ipo_quotation_release_time = std::byteswap(msg.ipo_quotation_release_time);
                msg.ipo_price = std::byteswap(msg.ipo_price);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'J': {
                file.seekg(-1, std::ios::cur);
                LuldAuctionCollarMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(LuldAuctionCollarMessage));

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
                file.seekg(-1, std::ios::cur);
                OperationalHaltMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(OperationalHaltMessage));

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'A': {
                file.seekg(-1, std::ios::cur);
                AddOrderMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(AddOrderMessage));

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
                file.seekg(-1, std::ios::cur);
                AddOrderMpidMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(AddOrderMpidMessage));

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.order_reference_number = std::byteswap(msg.order_reference_number);
                msg.shares = std::byteswap(msg.shares);
                msg.price = std::byteswap(msg.price);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'E': {
                file.seekg(-1, std::ios::cur);
                OrderExecutedMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(OrderExecutedMessage));

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
                file.seekg(-1, std::ios::cur);
                OrderExecutedWithPriceMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(OrderExecutedWithPriceMessage));

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
                file.seekg(-1, std::ios::cur);
                OrderCancelMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(OrderCancelMessage));

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.order_reference_number = std::byteswap(msg.order_reference_number);
                msg.canceled_shares = std::byteswap(msg.canceled_shares);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);

                market.process_cancel_order(msg);
                break;
            }
            case 'D': {
                file.seekg(-1, std::ios::cur);
                OrderDeleteMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(OrderDeleteMessage));

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.order_reference_number = std::byteswap(msg.order_reference_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'U': {
                file.seekg(-1, std::ios::cur);
                OrderReplaceMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(OrderReplaceMessage));

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
                file.seekg(-1, std::ios::cur);
                TradeMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(TradeMessage));

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
                file.seekg(-1, std::ios::cur);
                CrossTradeMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(CrossTradeMessage));

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.shares = std::byteswap(msg.shares);
                msg.cross_price = std::byteswap(msg.cross_price);
                msg.match_number = std::byteswap(msg.match_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'B': {
                file.seekg(-1, std::ios::cur);
                BrokenTradeMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(BrokenTradeMessage));

                msg.locate = std::byteswap(msg.locate);
                msg.tracking_number = std::byteswap(msg.tracking_number);
                msg.match_number = std::byteswap(msg.match_number);
                uint64_t updated_timestamp = parse_6byte_timestamp(msg.timestamp);
                break;
            }
            case 'I': {
                file.seekg(-1, std::ios::cur);
                NoiiMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(NoiiMessage));

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
                file.seekg(-1, std::ios::cur);
                DlcrMessage msg;
                file.read(reinterpret_cast<char*>(&msg), sizeof(DlcrMessage));

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
                file.seekg(message_length - 1, std::ios::cur);
                break;
            }

        }

    }

}



uint64_t Parser::get_message_count() {
    return message_count;
}
