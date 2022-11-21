#include <iostream>
#include <cstdint>
#include "InputParser.h"
#include "ModularArithmetic.h"
#include "Randomizer.h"

const uint64_t DEFAULT_SEED = 123;
const uint64_t DEFAULT_PUBLIC_BASE = 2;
const uint64_t DEFAULT_PUBLIC_MODULUS = 30803;

struct Args {
    uint64_t seed;
    uint64_t public_base;
    uint64_t public_modulus;
    uint64_t private_key_a;
    uint64_t private_key_b;
};

Args parseArgs(int argc, char **argv) {
    Args args = {
            .seed=DEFAULT_SEED,
            .public_base=DEFAULT_PUBLIC_BASE,
            .public_modulus=DEFAULT_PUBLIC_MODULUS,
            .private_key_a=0,
            .private_key_b=0
    };

    InputParser input(argc, argv);
    input.parseOption("-s", args.seed);
    input.parseOption("-p", args.public_modulus);
    input.parseOption("-g", args.public_base);
    input.parseOption("-xa", args.private_key_a);
    input.parseOption("-xb", args.private_key_b);

    return args;
}

uint64_t derivePublicKey(uint64_t private_key, uint64_t public_base, uint64_t public_modulus) {
    ModularArithmetic ma(public_modulus);
    return ma.pow(public_base, private_key);
}

uint64_t deriveSharedKey(uint64_t private_key, uint64_t public_key, uint64_t public_modulus) {
    ModularArithmetic ma(public_modulus);
    return ma.pow(public_key, private_key);
}

int main(int argc, char **argv) {
    Args args = parseArgs(argc, argv);
    Randomizer randomizer(args.seed);

    std::cout << "Randomizer seed = " << args.seed << std::endl;
    std::cout << "Public base (g) = " << args.public_base << std::endl;
    std::cout << "Public modulus (p) = " << args.public_modulus << std::endl;

    std::cout << "----- STEP 1 -----" << std::endl;

    uint64_t alice_private_key = args.private_key_a;
    if (alice_private_key == 0) {
        alice_private_key = randomizer.random(UINT16_MAX, UINT32_MAX);
    }
    std::cout << "Alice private key (x_a) = " << alice_private_key << std::endl;

    uint64_t bob_private_key = args.private_key_b;
    if (bob_private_key == 0) {
        bob_private_key = randomizer.random(UINT16_MAX, UINT32_MAX);
    }
    std::cout << "Bob private key (x_b) = " << bob_private_key << std::endl;

    std::cout << "----- STEP 2 -----" << std::endl;

    uint64_t alice_public_key = derivePublicKey(alice_private_key, args.public_base, args.public_modulus);
    std::cout << "Alice public key (y_a) = " << alice_public_key << std::endl;
    uint64_t bob_public_key = derivePublicKey(bob_private_key, args.public_base, args.public_modulus);
    std::cout << "Bob public key (y_b) = " << bob_public_key << std::endl;

    std::cout << "----- STEP 3 -----" << std::endl;

    uint64_t alice_shared_key = deriveSharedKey(alice_private_key, bob_public_key, args.public_modulus);
    std::cout << "Alice shared key (s_ab) = " << alice_shared_key << std::endl;
    uint64_t bob_shared_key = deriveSharedKey(bob_private_key, alice_public_key, args.public_modulus);
    std::cout << "Bob shared key (s_ba) = " << bob_shared_key << std::endl;
}
