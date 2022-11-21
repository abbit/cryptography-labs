#include <iostream>
#include <unordered_map>
#include "InputParser.h"
#include "ModularArithmetic.h"

const uint64_t DEFAULT_P = 30803;
const uint64_t DEFAULT_G = 2;

struct Args {
    uint64_t p;
    uint64_t g;
    uint64_t x;
};

Args parseArgs(int argc, char **argv) {
    Args args = {.p=DEFAULT_P, .g=DEFAULT_G, .x=0};
    InputParser input(argc, argv);
    input.parseOption("-p", args.p);
    input.parseOption("-g", args.g);
    input.parseOption("-x", args.x);
    if (args.x == 0) {
        std::cerr << "Exponent is required, use -x [exp]" << std::endl;
        exit(1);
    }
    if (args.x >= args.p) {
        std::cerr << "Exponent must be less than p" << std::endl;
        exit(1);
    }
    return args;
}

int main(int argc, char **argv) {
    Args args = parseArgs(argc, argv);
    std::cout << "Parameters:\n";

    ModularArithmetic ma(args.p);

    uint64_t p = args.p;
    uint64_t g = args.g;
    uint64_t x = args.x;

    uint64_t y = ma.pow(g, x);

    std::cout << "p = " << p << std::endl;
    std::cout << "g = " << g << std::endl;
    std::cout << "x = " << x << std::endl;
    std::cout << "y = " << y << std::endl;

    std::cout << "----- Cracking -----" << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();

    uint64_t n = (uint64_t) ceil(sqrt(p));
    std::cout << "n = m = " << n << std::endl;

    std::unordered_map<uint64_t, uint64_t> baby_steps;
    uint64_t baby_step = y;
    baby_steps.emplace(baby_step, 0);
    for (uint64_t i = 1; i < n; i++) {
        baby_step = ma.mul(baby_step, g); // y * g^i
        baby_steps.emplace(baby_step, i);
    }

    uint64_t start_giant_step = ma.pow(g, n);
    uint64_t giant_step = start_giant_step;
    for (uint64_t j = 1; j < n; j++) {
        if (baby_steps.find(giant_step) != baby_steps.end()) {
            uint64_t i = baby_steps[giant_step];
            uint64_t exponent = j * n - i;
            auto end_time = std::chrono::high_resolution_clock::now();
            std::cout << "Cracked in "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " ms!"
                      << " exponent was " << exponent << std::endl;
            break;
        }
        giant_step = ma.mul(giant_step, start_giant_step); // g^(j*n)
    }
}