#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <stdexcept>
#include <string>

#include "string.hpp"

namespace encoder {
String encode(const String& input);
String decode(const String& input);
}  // namespace encoder

#endif
