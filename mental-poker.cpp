#include <iostream>
#include <vector>
#include <tuple>
#include "InputParser.h"
#include "Randomizer.h"
#include "ModularArithmetic.h"

const int DEFAULT_SEED = 321;

const uint8_t CARD_A = 0b00;
const uint8_t CARD_B = 0b01;
const uint8_t CARD_C = 0b10;

std::string cardToStr(uint64_t card) {
    switch (card & 0b11) {
        case CARD_A:
            return "Card A";
        case CARD_B:
            return "Card B";
        case CARD_C:
            return "Card C";
        default:
            return "ERROR! This should not happen!";
    }
}

struct Args {
    uint64_t seed;
};

Args parseArgs(int argc, char **argv) {
    Args args = {.seed=DEFAULT_SEED};

    InputParser input(argc, argv);
    input.parseOption("-s", args.seed);

    return args;
}

std::tuple<uint64_t, uint64_t> generateKeys(uint64_t p, Randomizer &randomizer) {
    ModularArithmetic ma(p - 1);
    uint64_t d = randomizer.randomCoprime(0, UINT64_MAX, p - 1);
    uint64_t c = ma.inv(d);
    return std::make_tuple(d, c);
}

int main(int argc, char **argv) {
    Args args = parseArgs(argc, argv);
    Randomizer randomizer(args.seed);
    std::cout << "Randomizer seed = " << args.seed << std::endl;

    std::cout << "----- STEP 0 -----" << std::endl;

    uint64_t p = randomizer.randomPrime(2, UINT64_MAX);
    std::cout << "P = " << p << std::endl;

    auto [d_a, c_a] = generateKeys(p, randomizer);
    std::cout << "d_a = " << d_a << std::endl;
    std::cout << "c_a = " << c_a << std::endl;

    auto [d_b, c_b] = generateKeys(p, randomizer);
    std::cout << "d_b = " << d_b << std::endl;
    std::cout << "c_b = " << c_b << std::endl;

    std::cout << "----- STEP 1 -----" << std::endl;

    std::vector<uint64_t> cards(3);
    cards[0] = ((randomizer.random(1, p - 1 - 2) >> 2) << 2) | CARD_A;
    cards[1] = ((randomizer.random(1, p - 1 - 2) >> 2) << 2) | CARD_B;
    cards[2] = ((randomizer.random(1, p - 1 - 2) >> 2) << 2) | CARD_C;

    for (auto &card: cards) {
        std::cout << cardToStr(card) << " == " << card << "\n";
    }

    ModularArithmetic ma(p);

    for (auto &card: cards) {
        card = ma.pow(card, c_a);
    }
    randomizer.shuffle(cards);

    std::cout << "Alice sends Bob encrypted shuffled cards:\n";
    for (auto &card: cards) {
        std::cout << card << "\n";
    }

    std::cout << "----- STEP 2 -----" << std::endl;

    uint64_t alice_encrypted_card = randomizer.pick(cards);
    std::cout << "Bob sends Alice her encrypted card = " << alice_encrypted_card << "\n";

    uint64_t alice_card = ma.pow(alice_encrypted_card, d_a);
    std::cout << "Alice card is " << alice_card << " == " << cardToStr(alice_card) << "\n";

    std::cout << "----- STEP 3 -----" << std::endl;

    for (auto &card: cards) {
        card = ma.pow(card, c_b);
    }

    std::cout << "Bob sends Alice remaining encrypted cards:\n";
    for (auto &card: cards) {
        std::cout << card << "\n";
    }

    std::cout << "----- STEP 4 -----" << std::endl;

    uint64_t bob_encrypted_card = ma.pow(randomizer.pick(cards), d_a);
    std::cout << "Alice sends Bob his encrypted card = " << bob_encrypted_card << "\n";

    uint64_t bob_card = ma.pow(bob_encrypted_card, d_b);
    std::cout << "Bob card is " << bob_card << " == " << cardToStr(bob_card) << "\n";
}