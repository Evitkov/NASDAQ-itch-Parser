#include <chrono>
#include <iostream>
#include <string>

#include "Market.h"
#include "Parser.h"

int main(int argc, char* argv[]) {
    std::string filepath;

    // If the user passed a file path argument when running the program
    if (argc > 1) {
        filepath = argv[1];
    } else {
        std::cout << "[INFO] No arguments passed. Defaulting to hardcoded path.\n";
        filepath = "../data/08302019.NASDAQ_ITCH50";
    }

    Market nasdaq_market;
    Parser parser(filepath, nasdaq_market);

    auto start_time = std::chrono::high_resolution_clock::now();
    parser.parse();

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    uint64_t message_count = parser.get_message_count();

    std::cout << "\n[METRICS] Parse Complete.\n";
    std::cout << "Total Market Volume: " << nasdaq_market.get_total_volume() << " shares\n";
    std::cout << "Total Messages: " << message_count << '\n';
    std::cout << "Time Elapsed: " << elapsed.count() << " seconds\n";
    std::cout << "Throughput: " << (message_count / elapsed.count()) / 1000000.0 << " million msgs/sec\n";

    return 0;
}