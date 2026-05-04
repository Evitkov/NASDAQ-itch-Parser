#pragma once
#include "Market.h"
class Parser {
    public:
    Parser(std::string& file_path, Market& market)
    //initializer list needed for reference variables
        : filepath(file_path), market(market){}

    void parse();

    uint64_t get_message_count();


private:
    std::string& filepath;
    Market& market;
    uint64_t message_count{};
};
