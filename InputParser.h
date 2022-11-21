#pragma once

#include <vector>

class InputParser {
public:
    InputParser(int &argc, char **argv) {
        for (int i = 1; i < argc; ++i)
            tokens_.emplace_back(argv[i]);
    }

    const std::string &getOption(const std::string &option) const {
        std::vector<std::string>::const_iterator itr;
        itr = std::find(tokens_.begin(), tokens_.end(), option);
        if (itr != tokens_.end() && ++itr != tokens_.end()) {
            return *itr;
        }
        static const std::string empty_string;
        return empty_string;
    }

    bool isOptionExists(const std::string &option) const {
        return std::find(tokens_.begin(), tokens_.end(), option) != tokens_.end();
    }

    void parseOption(const std::string &option_name, uint64_t &option_value) const {
        if (isOptionExists(option_name)) {
            option_value = std::stoull(getOption(option_name));
        }
    }

private:
    std::vector<std::string> tokens_;
};