#pragma once

#include <cstdint>

class ModularArithmetic {
public:
    explicit ModularArithmetic(uint64_t modulus) : modulus_(modulus) {}

    uint64_t add(uint64_t a, uint64_t b) const {
        a %= modulus_;
        b %= modulus_;

        uint64_t res = a + b;
        if (res >= modulus_ || res < a) {
            res -= modulus_;
        }

        return res;
    }

    uint64_t sub(uint64_t a, uint64_t b) const {
        a %= modulus_;
        b %= modulus_;

        uint64_t res = a - b;
        if (res >= modulus_ || res > a) {
            res += modulus_;
        }

        return res;
    }

    uint64_t mul(uint64_t a, uint64_t b) const {
        if (b == 0) {
            return 0;
        }
        if (b == 1) {
            return a % modulus_;
        }

        uint64_t res = mul(a, b >> 1); // a * floor(b / 2)
        res = add(res, res);
        if (b & 1) { // если b нечетное, добавить еще один a
            res = add(res, a);
        }

        return res;
    }

    uint64_t pow(uint64_t base, uint64_t exponent) const {
        if (modulus_ == 1) {
            return 0;
        }
        if (exponent == 0) {
            return 1;
        }

        uint64_t res = 1;
        uint64_t curr = base;

        while (exponent > 0) {
            if (exponent & 1) {
                res = mul(res, curr);
            }

            curr = mul(curr, curr);
            exponent = exponent >> 1;
        }

        return res;
    }

    uint64_t gcdExtended(uint64_t a, uint64_t b, uint64_t &x, uint64_t &y) {
        if (b == 0) {
            x = 1;
            y = 0;
            return a;
        }

        uint64_t x1, y1;
        uint64_t d = gcdExtended(b, a % b, x1, y1);
        x = y1;
        y = sub(x1, mul(y1, (a / b)));

        return d;
    }

    uint64_t inv(uint64_t c) {
        uint64_t c_inv, _;
        uint64_t g = gcdExtended(c, modulus_, c_inv, _);
        assert(g == 1);
        return c_inv;
    }

private:
    uint64_t modulus_;
};