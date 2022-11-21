#include <iostream>
#include "InputParser.h"
#include "functions.h"
#include "Randomizer.h"
#include "ModularArithmetic.h"

const int DEFAULT_SEED = 123;
constexpr uint64_t Q_MAX = UINT64_MAX / 2 - 2;

struct Args {
    uint64_t seed;
};

Args parseArgs(int argc, char **argv) {
    Args args = {.seed=DEFAULT_SEED};
    InputParser input(argc, argv);
    input.parseOption("-s", args.seed);
    return args;
}

struct ElGamalParams {
    uint64_t modulus;
    uint64_t base;

    static ElGamalParams generate(Randomizer &randomizer) {
        ElGamalParams params;
        uint64_t prime_factor;
        do {
            prime_factor = randomizer.randomPrime(UINT16_MAX, Q_MAX);
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

/*
 * Алгоритм подбрасывания монетки (взят из книги "Введение в криптографию" Ященко):
 * 1. Алиса генерирует ключи по схеме Эль-Гамаля и отправляет публичный ключ Бобу
 * 2. Боб генерирует ключи по схеме Эль-Гамаля и случайный бит 'b', и отравляет Алисе r = (y_a)^b * y_b (mod P)
 * 3. Алиса генерирует случайный бит 'a' и отправляет его Бобу
 * 4. Боб отправляет Алисе ранее сгенерированные бит 'b' и свой секретный ключ x_b
 * 5. Алиса проверяет, что  r = (y_a)^b * y_b = (y_a)^b * (g)^x_b (mod P)
 * 6. Если все правильно, то результат подбрасывания монетки = a XOR b
 */

int main(int argc, char **argv) {
    Args args = parseArgs(argc, argv);
    Randomizer randomizer(args.seed);
    std::cout << "Randomizer seed = " << args.seed << std::endl;

    std::cout << "----- STEP 0 -----" << std::endl;

    ElGamalParams params = ElGamalParams::generate(randomizer);
    std::cout << "ElGamal parameters:" << std::endl;
    params.print();

    ModularArithmetic ma(params.modulus);

    std::cout << "----- STEP 1 -----" << std::endl;

    ElGamalKey alice_key = ElGamalKey::generate(params, randomizer);
    std::cout << "Alice key:" << std::endl;
    alice_key.print();

    std::cout << "----- STEP 2 -----" << std::endl;

    ElGamalKey bob_key = ElGamalKey::generate(params, randomizer);
    std::cout << "Bob key:" << std::endl;
    bob_key.print();

    uint64_t b = randomizer.random(0, 1);
    uint64_t r = ma.mul(ma.pow(alice_key.public_key, b), bob_key.public_key); // r = (y_a)^b * y_b (mod P)
    std::cout << "b = " << b << std::endl;
    std::cout << "r = " << r << std::endl;

    std::cout << "Bob sends r to Alice" << std::endl;

    std::cout << "----- STEP 3 -----" << std::endl;

    uint64_t a = randomizer.random(0, 1);
    std::cout << "'a' = " << a << std::endl;
    std::cout << "Alice sends 'a' to Bob" << std::endl;

    std::cout << "----- STEP 4 -----" << std::endl;

    std::cout << "Bob sends 'b' his private key to Alice" << std::endl;

    std::cout << "----- STEP 5 -----" << std::endl;

    uint64_t r_check = ma.mul(ma.pow(alice_key.public_key, b),
                              ma.pow(params.base, bob_key.private_key)); // (y_a)^b * (g)^x_b (mod P)
    if (r != r_check) {
        std::cout << "Bob trying to cheat!" << std::endl;
        return 1;
    }

    uint64_t result = a ^ b;
    std::cout << "Result = " << result << std::endl;

}
