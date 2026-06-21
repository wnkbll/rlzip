#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <string>
#include <stdexcept>

namespace encoder {
    std::string encode(const std::string& input);
    std::string decode(const std::string& input);
}

#endif
