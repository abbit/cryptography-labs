#include <iostream>
#include "InputParser.h"
#include "functions.h"
#include "Randomizer.h"
#include "rsa.h"

const int DEFAULT_SEED = 321;

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

struct SignedMessage {
    std::string message;
    uint64_t signature;
};

int main(int argc, char **argv) {
    Args args = parseArgs(argc, argv);
    Randomizer randomizer(args.seed);
    std::cout << "Randomizer seed = " << args.seed << std::endl;

    std::cout << "----- STEP 0 -----" << std::endl;

    RSAParams rsa_params = RSAParams::generate(randomizer);
    std::cout << "RSA params:\n";
    rsa_params.print();

    std::cout << "----- STEP 1 -----" << std::endl;

    SignedMessage signed_message;
    signed_message.message = args.message;
    signed_message.signature = signMessageRSA(hash(signed_message.message), rsa_params.private_key,
                                              rsa_params.public_modulus);

    std::cout << "Message: " << signed_message.message << std::endl;
    std::cout << "Hash(message) = " << hash(signed_message.message) << std::endl;
    std::cout << "signature = " << signed_message.signature << std::endl;

    std::cout << "----- STEP 2 -----" << std::endl;

    size_t message_hash_from_signature = getMessageHashRSA(signed_message.signature, rsa_params.public_key,
                                                           rsa_params.public_modulus);
    std::cout << "Message hash from signature = " << message_hash_from_signature << "\n";
    std::cout << "Received message hash = " << hash(signed_message.message) << "\n";

    if (hash(signed_message.message) == message_hash_from_signature) {
        std::cout << "Signature is valid\n";
    } else {
        std::cout << "Signature is invalid!\n";
    }
}
