#include <iostream>
#include "InputParser.h"
#include "functions.h"
#include "Randomizer.h"
#include "rsa.h"

const int DEFAULT_SEED = 123;
const uint64_t DEFAULT_P_A = 131;
const uint64_t DEFAULT_Q_A = 227;
const uint64_t DEFAULT_D_A = 3;
const uint64_t DEFAULT_P_B = 113;
const uint64_t DEFAULT_Q_B = 281;
const uint64_t DEFAULT_D_B = 3;

struct Args {
    uint64_t seed;
    uint64_t message;
    RSAParams a;
    RSAParams b;
};

Args parseArgs(int argc, char **argv) {
    Args args = {
            .seed=DEFAULT_SEED,
            .a{
                    .p=DEFAULT_P_A,
                    .q=DEFAULT_Q_A,
                    .public_key=DEFAULT_D_A,
            },
            .b{
                    .p=DEFAULT_P_B,
                    .q=DEFAULT_Q_B,
                    .public_key=DEFAULT_D_B,
            },
    };

    InputParser input(argc, argv);

    input.parseOption("-s", args.seed);
    input.parseOption("-pa", args.a.p);
    input.parseOption("-qa", args.a.q);
    input.parseOption("-da", args.a.public_key);
    input.parseOption("-pb", args.b.p);
    input.parseOption("-qb", args.b.q);
    input.parseOption("-db", args.b.public_key);
    input.parseOption("-m", args.message);

    return args;
}

int main(int argc, char **argv) {
    Args args = parseArgs(argc, argv);
    Randomizer randomizer(args.seed);
    std::cout << "Randomizer seed = " << args.seed << std::endl;

    RSAParams a = args.a;
    std::cout << "P_a = " << a.p << std::endl;
    std::cout << "Q_a = " << a.q << std::endl;

    RSAParams b = args.b;
    std::cout << "P_b = " << b.p << std::endl;
    std::cout << "Q_b = " << b.q << std::endl;

    std::cout << "----- STEP 0 -----" << std::endl;

    a.public_modulus = a.p * a.q;
    a.private_modulus = (a.p - 1) * (a.q - 1);
    if (a.public_key == 0) {
        a.public_key = generatePublicKeyRSA(a.private_modulus, randomizer);
    }
    a.private_key = derivePrivateKeyRSA(a.public_key, a.private_modulus);
    std::cout << "N_a = " << a.public_modulus << std::endl;
    std::cout << "f_a = " << a.private_modulus << std::endl;
    std::cout << "d_a = " << a.public_key << std::endl;
    std::cout << "c_a = " << a.private_key << std::endl;

    b.public_modulus = b.p * b.q;
    b.private_modulus = (b.p - 1) * (b.q - 1);
    if (b.public_key == 0) {
        b.public_key = generatePublicKeyRSA(b.private_modulus, randomizer);
    }
    b.private_key = derivePrivateKeyRSA(b.public_key, b.private_modulus);
    std::cout << "N_b = " << b.public_modulus << std::endl;
    std::cout << "f_b = " << b.private_modulus << std::endl;
    std::cout << "d_b = " << b.public_key << std::endl;
    std::cout << "c_b = " << b.private_key << std::endl;

    uint64_t message = args.message;
    if (message == 0) {
        message = randomizer.random(0, b.public_modulus - 1);
    }
    std::cout << "Message (m) = " << message << std::endl;

    std::cout << "----- STEP 1 -----" << std::endl;

    uint64_t encrypted_message = encryptMessageRSA(
            message, b.public_key, b.public_modulus);
    std::cout << "Alice sends Bob encrypted message (e) = " << encrypted_message << std::endl;

    std::cout << "----- STEP 2 -----" << std::endl;

    uint64_t decrypted_message = decryptMessageRSA(
            encrypted_message, b.private_key, b.public_modulus);
    std::cout << "Bob decrypts message (m') = " << decrypted_message << std::endl;
}
