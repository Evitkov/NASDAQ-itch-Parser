#pragma once
#include <cstdint>

#pragma pack(push, 1) // Disable padding so we can parse sequential bytes

// System Event Message ('S')
struct SystemEventMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char event_code;
};

// Stock Directory Message ('R')
struct StockDirectoryMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    char market_category;
    char financial_status_indicator;
    uint32_t round_lot_size;
    char round_lots_only;
    char issue_classification;
    char issue_subtype[2];
    char authenticity;
    char short_sale_threshold_indicator;
    char ipo_flag;
    char luld_reference_price_tier;
    char etp_flag;
    uint32_t etp_leverage_factor;
    char inverse_indicator;
};

// Stock Trading Action Message ('H')
struct StockTradingActionMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    char trading_state;
    char reserved;
    char reason[4];
};

// Reg SHO Restriction Message ('Y')
struct RegShoRestrictionMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    char reg_sho_action;
};

// Market Participant Position Message ('L')
struct MarketParticipantPositionMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char mpid[4];
    char stock[8];
    char primary_market_maker;
    char market_maker_mode;
    char market_participant_state;
};

// MWCB Decline Level Message ('V')
struct MwcbDeclineLevelMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t level_1;
    uint64_t level_2;
    uint64_t level_3;
};

// MWCB Status Message ('W')
struct MwcbStatusMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char breached_level;
};

// IPO Quoting Period Update Message ('K')
struct IpoQuotingPeriodUpdateMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    uint32_t ipo_quotation_release_time;
    char ipo_quotation_release_qualifier;
    uint32_t ipo_price;
};

// LULD Auction Collar Message ('J')
struct LuldAuctionCollarMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    uint32_t reference_price;
    uint32_t upper_collar;
    uint32_t lower_collar;
    uint32_t auction_collar_extension;
};

// Operational Halt Message ('h')
struct OperationalHaltMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    char market_code;
    char operational_halt_action;
};

// Add Order Message
struct AddOrderMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_reference_number;
    char buy_sell_indicator;
    uint32_t shares;
    char stock[8];
    uint32_t price;
};

// Add Order with MPID Attribution Message ('F')
struct AddOrderMpidMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_reference_number;
    char buy_sell_indicator;
    uint32_t shares;
    char stock[8];
    uint32_t price;
    char attribution[4];
};

// Order Executed Message ('E')
struct OrderExecutedMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_reference_number;
    uint32_t executed_shares;
    uint64_t match_number;
};

// Order Executed With Price Message ('C')
struct OrderExecutedWithPriceMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_reference_number;
    uint32_t executed_shares;
    uint64_t match_number;
    char printable;
    uint32_t execution_price;
};

// Order Cancel Message ('X')
struct OrderCancelMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_reference_number;
    uint32_t canceled_shares;
};

// Order Delete Message ('D')
struct OrderDeleteMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_reference_number;
};

// Order Replace Message ('U')
struct OrderReplaceMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t original_order_reference_number;
    uint64_t new_order_reference_number;
    uint32_t shares;
    uint32_t price;
};

// Trade Message (Non-Cross) ('P')
struct TradeMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_reference_number;
    char buy_sell_indicator;
    uint32_t shares;
    char stock[8];
    uint32_t price;
    uint64_t match_number;
};

// Cross Trade Message ('Q')
struct CrossTradeMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t shares;
    char stock[8];
    uint32_t cross_price;
    uint64_t match_number;
    char cross_type;
};

// Broken Trade Message ('B')
struct BrokenTradeMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t match_number;
};

// Net Order Imbalance Indicator (NOII) Message ('I')
struct NoiiMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t paired_shares;
    uint64_t imbalance_shares;
    char imbalance_direction;
    char stock[8];
    uint32_t far_price;
    uint32_t near_price;
    uint32_t current_reference_price;
    char cross_type;
    char price_variation_indicator;
};

// Direct Listing with Capital Raise Price Discovery Message ('O')
struct DlcrMessage {
    char message_type;
    uint16_t locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    char open_eligibility_status;
    uint32_t minimum_allowable_price;
    uint32_t maximum_allowable_price;
    uint32_t near_execution_price;
    uint64_t near_execution_time;
    uint32_t lower_price_range_collar;
    uint32_t upper_price_range_collar;
};

#pragma pack(pop) // Restore default padding rules