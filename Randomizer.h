#pragma once

#include <random>
#include "functions.h"

class Randomizer {
public:
    explicit Randomizer(uint64_t seed) : mt_(std::mt19937_64(seed)) {}

    // random int from [min, max]
    uint64_t random(uint64_t min, uint64_t max) {
        std::uniform_int_distribution<uint64_t> dist(min, max);
        return dist(mt_);
    }

    uint64_t randomPrime(uint64_t min, uint64_t max) {
        uint64_t res;
        do {
            res = random(min, max);
        } while (!isPrime(res));

        return res;
    }

    uint64_t randomCoprime(uint64_t min, uint64_t max, uint64_t b) {
        uint64_t res;
        do {
            res = random(min, max);
        } while (gcd(res, b) != 1);

        return res;
    }

    void shuffle(std::vector<uint64_t> &vec) {
        std::shuffle(vec.begin(), vec.end(), mt_);
    }

    uint64_t pick(std::vector<uint64_t> &vec) {
        shuffle(vec);
        uint64_t res = vec.back();
        vec.pop_back();
        return res;
    }

private:
    std::mt19937_64 mt_;
};