#include <iostream>
#include "InputParser.h"
#include "functions.h"
#include "Randomizer.h"
#include "ModularArithmetic.h"

const int DEFAULT_SEED = 321;
constexpr uint64_t Q_MAX = UINT64_MAX / 2 - 2;

struct Args {
    uint64_t seed;
    std::string message;
};

Args parseArgs(int argc, char **argv) {
    Args args = {.seed=DEFAULT_SEED};

    InputParser input(argc, argv);

    input.parseOption("-s", args.seed);
    args.message = input.getOption("-m");

    return args;
}

struct ElGamalParams {
    uint64_t modulus;
    uint64_t base;

    static ElGamalParams generate(Randomizer &randomizer) {
        ElGamalParams params;
        uint64_t prime_factor;
        do {
            prime_factor = randomizer.randomPrime(UINT32_MAX, Q_MAX);
            params.modulus = 2 * prime_factor + 1;
        } while (!isPrime(params.modulus));

        ModularArithmetic ma(params.modulus);
        do {
            params.base = randomizer.random(2, params.modulus - 2);
        } while (ma.pow(params.base, prime_factor) == 1);

        return params;
    }

    void print() {
        std::cout << "modulus = " << modulus << std::endl;
        std::cout << "base = " << base << std::endl;
    }
};

struct ElGamalKey {
    uint64_t private_key;
    uint64_t public_key;

    static ElGamalKey generate(const ElGamalParams &params, Randomizer &randomizer) {
        ElGamalKey key;
        ModularArithmetic ma(params.modulus);

        key.private_key = randomizer.random(2, params.modulus - 2);
        key.public_key = ma.pow(params.base, key.private_key);

        return key;
    }

    void print() {
        std::cout << "private key = " << private_key << std::endl;
        std::cout << "public key = " << public_key << std::endl;
    }
};

struct SignedMessage {
    std::string message;
    uint64_t r;
    uint64_t s;

    void print() {
        std::cout << "message = " << message << std::endl;
        std::cout << "Hash(message) = " << hash(message) << std::endl;
        std::cout << "r = " << r << std::endl;
        std::cout << "s = " << s << std::endl;
    }
};

bool isSignatureValid(const SignedMessage &signed_message, const ElGamalParams &params, uint64_t public_key) {
    ModularArithmetic ma(params.modulus);
    uint64_t lhs = ma.pow(params.base, hash(signed_message.message)); // g^h(m) mod p
    uint64_t rhs = ma.mul(ma.pow(public_key, signed_message.r),
                          ma.pow(signed_message.r, signed_message.s)); // y^r * r^s mod p

    return lhs == rhs;
}

int main(int argc, char **argv) {
    Args args = parseArgs(argc, argv);
    Randomizer randomizer(args.seed);
    std::cout << "Randomizer seed = " << args.seed << std::endl;

    std::cout << "----- STEP 0 -----" << std::endl;

    ElGamalParams params = ElGamalParams::generate(randomizer);
    std::cout << "ElGamal params:\n";
    params.print();

    std::cout << "----- STEP 1 - Sign and send message -----" << std::endl;

    ElGamalKey key = ElGamalKey::generate(params, randomizer);
    std::cout << "ElGamal key:\n";
    key.print();

    SignedMessage signed_message;
    signed_message.message = args.message;
    uint64_t signature_private_key = randomizer.randomCoprime(2, params.modulus - 2, params.modulus - 1);
    signed_message.r = ModularArithmetic(params.modulus).pow(params.base, signature_private_key);

    ModularArithmetic ma(params.modulus - 1);

    uint64_t u = ma.sub(hash(signed_message.message),
                        ma.mul(key.private_key, signed_message.r)); // u = (h(m) - x * r) mod (p-1)
    signed_message.s = ma.mul(ma.inv(signature_private_key), u); // s = (k^-1 * u) mod (p-1)

    std::cout << "Signed message:\n";
    signed_message.print();

    std::cout << "----- STEP 2 - Verify signature -----" << std::endl;

    std::cout << "Received message hash = " << hash(signed_message.message) << "\n";

    if (isSignatureValid(signed_message, params, key.public_key)) {
        std::cout << "Signature is valid\n";
    } else {
        std::cout << "Signature is invalid!\n";
    }
}
