#include <cstdint>
#include <iostream>
#include "InputParser.h"
#include "Randomizer.h"
#include "ModularArithmetic.h"

const uint64_t DEFAULT_SEED = 123;
const uint64_t DEFAULT_PUBLIC_BASE = 2;
const uint64_t DEFAULT_PUBLIC_MODULUS = 30803;

struct Args {
    uint64_t seed;
    uint64_t public_base;
    uint64_t public_modulus;
    uint64_t private_key_b;
    uint64_t session_private_key;
    uint64_t message;
};

Args parseArgs(int argc, char **argv) {
    Args args = {
            .seed = DEFAULT_SEED,
            .public_base = DEFAULT_PUBLIC_BASE,
            .public_modulus = DEFAULT_PUBLIC_MODULUS,
            .private_key_b = 0,
            .session_private_key = 0,
            .message = 0,
    };

    InputParser input(argc, argv);
    input.parseOption("-s", args.seed);
    input.parseOption("-p", args.public_modulus);
    input.parseOption("-g", args.public_base);
    input.parseOption("-cb", args.private_key_b);
    input.parseOption("-k", args.session_private_key);
    input.parseOption("-m", args.message);

    return args;
}

uint64_t derivePublicKey(uint64_t private_key, uint64_t public_base, uint64_t public_modulus) {
    ModularArithmetic ma(public_modulus);
    return ma.pow(public_base, private_key);
}

uint64_t encryptMessage(
        uint64_t message,
        uint64_t session_private_key,
        uint64_t public_key,
        uint64_t public_modulus
) {
    ModularArithmetic ma(public_modulus);
    return ma.mul(message, ma.pow(public_key, session_private_key));
}

uint64_t decryptMessage(
        uint64_t encrypted_message,
        uint64_t session_public_key,
        uint64_t private_key,
        uint64_t public_modulus
) {
    ModularArithmetic ma(public_modulus);
    return ma.mul(encrypted_message, ma.pow(session_public_key, -private_key + public_modulus - 1));
}

int main(int argc, char **argv) {
    Args args = parseArgs(argc, argv);
    Randomizer randomizer(args.seed);
    if (args.message >= args.public_modulus || args.message == 0) {
        args.message = randomizer.random(1, args.public_modulus - 1);
    }

    std::cout << "Randomizer seed = " << args.seed << std::endl;
    std::cout << "Public base (g) = " << args.public_base << std::endl;
    std::cout << "Public modulus (p) = " << args.public_modulus << std::endl;
    std::cout << "Message (m) = " << args.message << std::endl;

    std::cout << "----- STEP 1 -----" << std::endl;

    uint64_t bob_private_key = args.private_key_b;
    if (bob_private_key == 0) {
        bob_private_key = randomizer.random(2, args.public_modulus - 2);
    }
    std::cout << "Bob private key (c_b) = " << bob_private_key << std::endl;

    uint64_t bob_public_key = derivePublicKey(bob_private_key, args.public_base, args.public_modulus);
    std::cout << "Bob public key (d_b) = " << bob_public_key << std::endl;

    std::cout << "----- STEP 2 -----" << std::endl;

    uint64_t session_private_key = args.session_private_key;
    if (session_private_key == 0) {
        session_private_key = randomizer.random(2, args.public_modulus - 2);
    }
    std::cout << "Alice session private key (k) = " << session_private_key << std::endl;

    uint64_t session_public_key = derivePublicKey(session_private_key, args.public_base, args.public_modulus);
    uint64_t encrypted_message = encryptMessage(args.message, session_private_key, bob_public_key, args.public_modulus);
    std::cout << "Alice sends Bob a pair (session_public_key, encrypted_message) = " << session_public_key << ", "
              << encrypted_message << std::endl;

    std::cout << "----- STEP 3 -----" << std::endl;
    uint64_t decrypted_message = decryptMessage(encrypted_message, session_public_key, bob_private_key,
                                                args.public_modulus);
    std::cout << "Bob decrypts message, m = " << decrypted_message << std::endl;
}
