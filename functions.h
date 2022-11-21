#pragma once

#include <cstdint>
#include <vector>

uint64_t gcd(uint64_t a, uint64_t b) {
    while (b > 0) {
        a = a % b;
        std::swap(a, b);
    }
    return a;
}

// Euler totient function
uint64_t phi(uint64_t n) {
    uint64_t count = 1;
    for (uint64_t i = 2; i < n; ++i) {
        if (gcd(i, n) == 1) {
            count++;
        }
    }
    return count;
}

bool isPrime(uint64_t n) {
    if (n == 2 || n == 3) return true;

    if (n <= 1 || n % 2 == 0 || n % 3 == 0) return false;

    for (uint64_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }

    return true;
}

size_t hash(const std::string &message) {
    return std::hash<std::string>{}(message) % UINT32_MAX;
}

bool isInVector(std::vector<uint64_t> &v, uint64_t x) {
    for (uint64_t i: v) {
        if (i == x) {
            return true;
        }
    }
    return false;
}