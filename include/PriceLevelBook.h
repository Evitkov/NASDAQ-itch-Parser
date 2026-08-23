
#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>

struct PriceLevel {
    uint32_t price{0};
    uint64_t shares{0};
};

class PriceLevelBook {
public:
    inline static std::vector<PriceLevelBook*> all_books;
    inline size_t current_size() const { return prices.size(); }
    inline static size_t max_observed_levels = 0;
    PriceLevelBook() {
        prices.reserve(256);
        all_books.push_back(this);
    }

    inline void add(uint32_t price, uint32_t shares) {
        //iterator with custom comparator
        auto it = std::lower_bound(prices.begin(), prices.end(), price,
            [](const PriceLevel& lvl, uint32_t p) {
                return lvl.price < p;
            });

        if (it != prices.end() && it->price == price) {
            PriceLevel& level = *it;
            level.shares += shares;
        } else {
            prices.insert(it, PriceLevel{price, shares});
        }
    }

    inline void remove(uint32_t price, uint32_t shares) {
        auto it = std::lower_bound(prices.begin(), prices.end(), price,
                    [](const PriceLevel& lvl, uint32_t p) {
                        return lvl.price < p;
                    });

        if (it != prices.end() && it->price == price) {
            PriceLevel& level = *it;
            if (shares >= level.shares) {
                prices.erase(it);
            } else {
                level.shares -= shares;
            }
        }
    }
    inline bool empty() const { return prices.empty(); }

    // price level functions could be used for the market later as best ask best bid
    inline const PriceLevel* get_min_price() const {
        return prices.empty() ? nullptr : &prices.back();
    }
    inline const PriceLevel* get_max_price() const {
        return prices.empty() ? nullptr : &prices.front();
    }


private:
    std::vector<PriceLevel> prices;
};