#include <chrono>
#include <iostream>
#include <string>
#include <stdexcept>

#include "Market.h"
#include "Parser.h"
#include "OrderBook.h"

int main(int argc, char* argv[]) {
    std::string filepath;

    // If the user passed a file path argument when running the program
    if (argc > 1) {
        filepath = argv[1];
    } else {
        std::cout << "[INFO] No arguments passed. Defaulting to hardcoded path.\n";
        filepath = "../data/08302019.NASDAQ_ITCH50";
    }

    try {
        Market nasdaq_market;
        Parser parser(filepath, nasdaq_market);

        auto start_time = std::chrono::high_resolution_clock::now();
        parser.parse();
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;

        uint64_t message_count = parser.get_message_count();

        // Final Metrics Printout
        std::cout << "\n[METRICS] Parse Complete.\n";
        std::cout << "Total Market Volume: " << nasdaq_market.get_total_volume() << " shares\n";
        std::cout << "Total Messages: " << message_count << '\n';
        std::cout << "Time Elapsed: " << elapsed.count() << " seconds\n";
        std::cout << "Throughput: " << (message_count / elapsed.count()) / 1000000.0 << " million msgs/sec\n";

    } catch (const std::exception& e) {
        std::cerr << "\n[CRASH DUMP] Exception caught: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n[CRASH DUMP] Unknown exception caught!\n";
        return 1;
    }

    return 0;
}