#include <iostream>
#include "InputParser.h"
#include "Randomizer.h"
#include "ModularArithmetic.h"

const int DEFAULT_SEED = 123;
const unsigned char ALPHABET_START = 32; // first printable ASCII character
const unsigned char ALPHABET_END = 126; // last printable ASCII character
constexpr unsigned char ALPHABET_SIZE = ALPHABET_END - ALPHABET_START + 1;
ModularArithmetic ma(ALPHABET_SIZE);

struct Args {
    uint64_t seed;
    std::string message;
};

Args parseArgs(int argc, char **argv) {
    Args args = {.seed=DEFAULT_SEED};
    InputParser input(argc, argv);

    input.parseOption("-s", args.seed);

    args.message = input.getOption("-m");
    if (args.message.empty()) {
        std::cerr << "Message is required, use -m [message]" << std::endl;
        exit(1);
    }
    for (char &c: args.message) {
        if (c < ALPHABET_START || c > ALPHABET_END) {
            std::cerr << "Message must be printable ASCII characters" << std::endl;
            exit(1);
        }
    }

    return args;
}

std::string generate_key(size_t length, Randomizer &randomizer) {
    std::cout << "Generating random key of length " << length << std::endl;
    std::cout << "Key alphabet - 95 printable ASCII characters" << std::endl;
    std::string key;
    for (size_t i = 0; i < length; ++i) {
        key += randomizer.random(ALPHABET_START, ALPHABET_END);
    }

    return key;
}

std::string encrypt(const std::string &message, const std::string &key) {
    std::string encrypted;
    for (size_t i = 0; i < message.length(); ++i) {
        encrypted += ma.add(message[i] - ALPHABET_START, key[i] - ALPHABET_START) + ALPHABET_START;
    }
    return encrypted;
}

std::string decrypt(const std::string &encrypted, const std::string &key) {
    std::string message;
    for (size_t i = 0; i < encrypted.length(); ++i) {
        message += ma.sub(encrypted[i] - ALPHABET_START, key[i] - ALPHABET_START) + ALPHABET_START;
    }
    return message;
}

int main(int argc, char **argv) {
    Args args = parseArgs(argc, argv);
    Randomizer randomizer(args.seed);

    std::cout << "Randomizer seed = " << args.seed << std::endl;
    std::cout << "Message: \"" << args.message  << "\" (" << args.message.length() << " characters)" << std::endl;
    std::string key = generate_key(args.message.length(), randomizer);
    std::cout << "Key: \"" << key  << "\"" << std::endl;

    std::cout << "----- ENCRYPTION -----" << std::endl;

    std::string encrypted = encrypt(args.message, key);
    std::cout << "Encrypted: \"" << encrypted << "\"" << std::endl;

    std::cout << "----- DECRYPTION -----" << std::endl;

    std::string decrypted = decrypt(encrypted, key);
    std::cout << "Decrypted: \"" << decrypted << "\"" << std::endl;
}