#include <iostream>
#include "InputParser.h"
#include "functions.h"
#include "Randomizer.h"
#include "ModularArithmetic.h"
#include "rsa.h"

const int DEFAULT_SEED = 123;
const uint64_t BANKNOTE_VALUE = 100;

struct Args {
    uint64_t seed;
};

Args parseArgs(int argc, char **argv) {
    Args args = {.seed=DEFAULT_SEED};
    InputParser input(argc, argv);
    input.parseOption("-signature", args.seed);
    return args;
}

struct Banknote {
    uint64_t banknote_number;
    uint64_t signature;
};

class Bank {
public:
    RSAParams rsa_params;

    Bank(Randomizer &randomizer) : rsa_params(RSAParams::generate(randomizer)) {
        std::cout << "Bank RSA params:" << std::endl;
        rsa_params.print();
    }

    uint64_t createBankAccount(uint64_t initial_value) { // создаю клиента банка с фантиками на счету
        account_values_.push_back(initial_value);
        return account_values_.size() - 1;
    }

    Banknote signBanknote(size_t bank_account_number, uint64_t banknote_number) {
        assert(bank_account_number < account_values_.size()); // проверка на то, что счет существует
        assert(account_values_[bank_account_number] >= BANKNOTE_VALUE);
        account_values_[bank_account_number] -= BANKNOTE_VALUE;

        Banknote banknote;
        banknote.banknote_number = banknote_number;
        banknote.signature = signMessageRSA(banknote.banknote_number, rsa_params.private_key,
                                            rsa_params.public_modulus);
        std::cout << "Bank signed banknote " << banknote_number << " with signature "
                  << banknote.signature << std::endl;
        return banknote;
    }

    bool checkBanknoteSignature(const Banknote &banknote) {
        std::cout << "Bank checks banknote " << banknote.banknote_number << " with signature "
                  << banknote.signature << std::endl;
        if (!checkSignatureRSA(banknote.banknote_number, banknote.signature, rsa_params.public_key,
                               rsa_params.public_modulus)) {
            std::cout << "Banknote signature is invalid" << std::endl;
            return false;
        }

        if (isInVector(used_banknotes_, banknote.banknote_number)) {
            std::cout << "Banknote " << banknote.banknote_number << " is already used" << std::endl;
            return false;
        }

        used_banknotes_.push_back(banknote.banknote_number);

        return true;
    }

    bool addBanknoteToAccount(size_t bank_account_number, const Banknote &banknote) {
        if (!checkBanknoteSignature(banknote)) {
            return false;
        }

        account_values_[bank_account_number] += BANKNOTE_VALUE;
        return true;
    }

    void printAccountValues() {
        std::cout << "Account values:" << std::endl;
        for (size_t i = 0; i < account_values_.size(); ++i) {
            std::cout << "Account " << i << ": " << account_values_[i] << std::endl;
        }
    }

private:
    std::vector<uint64_t> account_values_;
    std::vector<uint64_t> used_banknotes_;
};

class Customer {
public:
    Customer(Randomizer &randomizer, Bank &bank) : randomizer_(randomizer),
                                                   bank_(bank),
                                                   bank_account_number_(bank.createBankAccount(150)) {}

    Banknote getBanknoteFromBank() {
        ModularArithmetic ma(bank_.rsa_params.public_modulus);
        uint64_t n = randomizer_.random(2, bank_.rsa_params.public_modulus - 1);
        n = n & (~0b1111); // сделать последние 4 бита нулями, для защиты от фабрикования банкнот
        std::cout << "Customer generated banknote number = " << n << std::endl;
        uint64_t r = randomizer_.randomCoprime(1, bank_.rsa_params.public_modulus - 1, bank_.rsa_params.public_modulus);
        uint64_t n1 = ma.mul(n, ma.pow(r, bank_.rsa_params.public_key)); // n * r^d mod N
        uint64_t s = bank_.signBanknote(bank_account_number_, n1).signature;
        uint64_t rr = ma.inv(r);
        uint64_t s1 = ma.mul(s, rr); // s * r^-1 mod N
        std::cout << "Customer derived signature = " << s1 << std::endl;

        Banknote banknote;
        banknote.banknote_number = n;
        banknote.signature = s1;
        return banknote;
    }

private:
    Randomizer &randomizer_;
    Bank &bank_;
    uint64_t bank_account_number_;
};

class Shop {
public:
    Shop(Bank &bank) : bank_(bank),
                       bank_account_number_(bank.createBankAccount(0)) {}

    bool acceptPayment(const Banknote &banknote) {
        std::cout << "Shop got banknote " << banknote.banknote_number << " with signature "
                  << banknote.signature << std::endl;
        if (bank_.addBanknoteToAccount(bank_account_number_, banknote)) {
            std::cout << "Banknote is valid, payment successful" << std::endl;
            return true;
        }

        std::cout << "Banknote is invalid, payment unsuccessful!" << std::endl;
        return false;
    }

private:
    Bank &bank_;
    uint64_t bank_account_number_;
};


int main(int argc, char **argv) {
    Args args = parseArgs(argc, argv);
    Randomizer randomizer(args.seed);
    std::cout << "Randomizer seed = " << args.seed << std::endl;

    std::cout << "----- STEP 0 -----" << std::endl;

    Bank bank(randomizer);
    Customer customer = Customer(randomizer, bank);
    Shop shop = Shop(bank);

    bank.printAccountValues();

    std::cout << "----- STEP 1 -----" << std::endl;

    Banknote banknote = customer.getBanknoteFromBank();

    bank.printAccountValues();

    std::cout << "----- STEP 2 -----" << std::endl;

    shop.acceptPayment(banknote);

    bank.printAccountValues();
}
