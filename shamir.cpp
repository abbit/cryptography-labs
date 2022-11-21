#include <iostream>
#include <cstdint>
#include <tuple>
#include <utility> // for std::swap, std::pair
#include "InputParser.h"
#include "functions.h"
#include "Randomizer.h"
#include "ModularArithmetic.h"

const uint64_t DEFAULT_SEED = 123;
const uint64_t DEFAULT_PUBLIC_MODULUS = 30803;

struct Args {
    uint64_t seed;
    uint64_t public_modulus;
    uint64_t message;
};

Args parseArgs(int argc, char **argv) {
    Args args = {
            .seed=DEFAULT_SEED,
            .public_modulus=DEFAULT_PUBLIC_MODULUS,
            .message=0,
    };

    InputParser input(argc, argv);
    input.parseOption("-s", args.seed);
    input.parseOption("-p", args.public_modulus);
    input.parseOption("-message", args.message);

    return args;
}

std::pair<uint64_t, uint64_t> generatePrivateKeyPair(uint64_t public_modulus, Randomizer &randomizer) {
    uint64_t c = 0;
    while (c % 2 == 0 || gcd(public_modulus - 1, c) != 1) {
        c = randomizer.random(1, public_modulus - 1);
    }
    ModularArithmetic ma(public_modulus - 1);
    uint64_t d = ma.inv(c);

    return std::make_pair(c, d);
}

int main(int argc, char **argv) {
    Args args = parseArgs(argc, argv);
    Randomizer randomizer(args.seed);

    uint64_t message = args.message;
    if (message >= args.public_modulus || message == 0) {
        message = randomizer.random(1, args.public_modulus - 1);
    }

    std::cout << "Randomizer seed = " << args.seed << std::endl;
    std::cout << "Public modulus (p) = " << args.public_modulus << std::endl;
    std::cout << "Message (m) = " << message << std::endl;

    std::cout << "----- STEP 0 -----" << std::endl;

    auto [c_a, d_a] = generatePrivateKeyPair(args.public_modulus, randomizer);
    std::cout << "Alice private key pair (c_a, d_a) = " << c_a << ", " << d_a << std::endl;

    auto [c_b, d_b] = generatePrivateKeyPair(args.public_modulus, randomizer);
    std::cout << "Bob private key pair (c_b, d_b) = " << c_b << ", " << d_b << std::endl;

    ModularArithmetic ma(args.public_modulus);

    std::cout << "----- STEP 1 -----" << std::endl;
    uint64_t x1 = ma.pow(message, c_a);
    std::cout << "Alice sending x_1 = " << x1 << " to Bob" << std::endl;

    std::cout << "----- STEP 2 -----" << std::endl;
    uint64_t x2 = ma.pow(x1, c_b);
    std::cout << "Bob sending x_2 = " << x2 << " to Alice" << std::endl;

    std::cout << "----- STEP 3 -----" << std::endl;
    uint64_t x3 = ma.pow(x2, d_a);
    std::cout << "Alice sending x_3 = " << x3 << " to Bob" << std::endl;

    std::cout << "----- STEP 4 -----" << std::endl;
    uint64_t x4 = ma.pow(x3, d_b);
    std::cout << "Bob calculating x_4 = message = " << x4 << std::endl;
}
