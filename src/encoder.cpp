#include "encoder.hpp"

namespace encoder {
    std::string encode(const std::string& input) {
        if (input.empty()) return "";
    
        std::string encoded = "";
        int count = 1;
    
        for (size_t i = 1; i <= input.length(); ++i) {
            if (i < input.length() && input[i] == input[i - 1]) {
                count++;
            } else {
                encoded += std::to_string(count) + input[i - 1];
                count = 1;
            }
        }
        return encoded;
    }
    
    std::string decode(const std::string& input) {
        if (input.empty()) return "";
    
        std::string decoded = "";
        std::string count_str = "";
    
        for (char ch : input) {
            if (std::isdigit(ch)) {
                count_str += ch;
            } else {
                if (!count_str.empty()) {
                    int count = std::stoi(count_str);
                    decoded.append(count, ch);
                    count_str = "";
                }
            }
        }
        return decoded;
    }
}
