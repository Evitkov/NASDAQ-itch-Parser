#include <iostream>

#include "Market.h"
#include "chrono"
#include "Parser.h"


int main() {
    //relative path one layer up specified with two dots
    std::string filepath = "../data/08302019.NASDAQ_ITCH50";

    Market nasdaq_market;
    Parser parser(filepath, nasdaq_market);

    auto start_time = std::chrono::high_resolution_clock::now();
    parser.parse();

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    uint64_t message_count = parser.get_message_count();

    std::cout << "\n[METRICS] Parse Complete." << std::endl;
    std::cout << "Total Market Volume: " << nasdaq_market.get_total_volume() << " shares" << std::endl;
    std::cout << "Total Messages: " << message_count << std::endl;
    std::cout << "Time Elapsed: " << elapsed.count() << " seconds" << std::endl;
    std::cout << "Throughput: " << (message_count / elapsed.count()) / 1000000.0 << " million msgs/sec" << std::endl;

    return 0;
}
