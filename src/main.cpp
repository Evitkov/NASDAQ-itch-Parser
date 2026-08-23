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

        std::cout << "\n[METRICS] Parse Complete.\n";
        std::cout << "Total Market Volume: " << nasdaq_market.get_total_volume() << " shares\n";
        std::cout << "Total Messages: " << message_count << '\n';
        std::cout << "Time Elapsed: " << elapsed.count() << " seconds\n";
        std::cout << "Throughput: " << (message_count / elapsed.count()) / 1000000.0 << " million msgs/sec\n";
        std::cout << "Peak orders: " << OrderBook::peak_orders<<"\n";
        std::vector<size_t> final_sizes;
        double sum = 0;

        // Collect the sizes of all books that actually had data
        for (auto* book : PriceLevelBook::all_books) {
            size_t s = book->current_size();
            if (s > 0) {
                final_sizes.push_back(s);
                sum += s;
            }
        }

        if (!final_sizes.empty()) {
            // Sort them from smallest to largest
            std::sort(final_sizes.begin(), final_sizes.end());

            size_t count = final_sizes.size();
            size_t median = final_sizes[count / 2];
            size_t p90 = final_sizes[count * 0.90]; // 90th percentile
            size_t p99 = final_sizes[count * 0.99]; // 99th percentile
            double avg = sum / count;

            std::cout << "\n--- Book Size Profiling ---\n";
            std::cout << "Total Active Books: " << count << "\n";
            std::cout << "Average Size: " << avg << "\n";
            std::cout << "Median Size:  " << median << "\n";
            std::cout << "90th Percentile: " << p90 << "\n";
            std::cout << "99th Percentile: " << p99 << "\n";
            std::cout << "Max Size: " << final_sizes.back() << "\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "\n[CRASH DUMP] Exception caught: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n[CRASH DUMP] Unknown exception caught!\n";
        return 1;
    }

    return 0;
}