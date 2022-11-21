#pragma once

#include <cstdint>
#include "ModularArithmetic.h"

uint64_t generatePublicKeyRSA(uint64_t private_modulus, Randomizer &randomizer) {
    return randomizer.randomCoprime(2, private_modulus - 1, private_modulus);
}

uint64_t derivePrivateKeyRSA(uint64_t public_key, uint64_t private_modulus) {
    ModularArithmetic ma(private_modulus);
    return ma.inv(public_key);
}

uint64_t encryptMessageRSA(uint64_t message, uint64_t public_key, uint64_t public_modulus) {
    ModularArithmetic ma(public_modulus);
    return ma.pow(message, public_key);
}

uint64_t decryptMessageRSA(uint64_t encrypted_message, uint64_t private_key, uint64_t public_modulus) {
    ModularArithmetic ma(public_modulus);
    return ma.pow(encrypted_message, private_key);
}

uint64_t signMessageRSA(uint64_t message_hash, uint64_t private_key, uint64_t public_modulus) {
    ModularArithmetic ma(public_modulus);
    return ma.pow(message_hash, private_key);
}

uint64_t getMessageHashRSA(uint64_t signature, uint64_t public_key, uint64_t public_modulus) {
    ModularArithmetic ma(public_modulus);
    return ma.pow(signature, public_key);
}

uint64_t checkSignatureRSA(uint64_t message_hash, uint64_t signature, uint64_t public_key, uint64_t public_modulus) {
    return getMessageHashRSA(signature, public_key, public_modulus) == message_hash;
}

struct RSAParams {
    uint64_t p;
    uint64_t q;
    uint64_t public_key;
    uint64_t private_key;
    uint64_t private_modulus;
    uint64_t public_modulus;

    static RSAParams generate(Randomizer &randomizer) {
        RSAParams params;
        params.p = randomizer.randomPrime(UINT16_MAX, UINT32_MAX);
        params.q = randomizer.randomPrime(UINT16_MAX, UINT32_MAX);
        params.public_modulus = params.p * params.q;
        params.private_modulus = (params.p - 1) * (params.q - 1);
        params.public_key = generatePublicKeyRSA(params.private_modulus, randomizer);
        params.private_key = derivePrivateKeyRSA(params.public_key, params.private_modulus);

        return params;
    }

    void print() const {
        std::cout << "p = " << p << std::endl;
        std::cout << "q = " << q << std::endl;
        std::cout << "N = " << public_modulus << std::endl;
        std::cout << "f = " << private_modulus << std::endl;
        std::cout << "d = " << public_key << std::endl;
        std::cout << "c = " << private_key << std::endl;
    }
};