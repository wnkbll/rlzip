#ifndef STRING_HPP
#define STRING_HPP

#include <cstring>
#include <filesystem>
#include <iostream>
#include <stdexcept>

class String {
   private:
    char* data;
    size_t len;
    size_t capacity;

   public:
    String();

    String(const char* s);

    ~String();

    String(size_t n, char c);

    String(const String& other);

    char* begin();
    char* end();

    const char* begin() const;
    const char* end() const;

    operator std::string() const;

    operator std::filesystem::path() const;

    String& operator=(const String& other);

    bool operator==(const char* other);

    bool operator!=(const char* other);

    String operator+(const std::string& s);

    String& operator+=(const std::string& s);

    char& operator[](size_t i);

    const char& operator[](size_t i) const;

    size_t length() const;

    bool empty() const;

    void push_back(char c);

    void append(size_t count, char c);

    const char* c_str() const;
};

inline std::istream& operator>>(std::istream& is, String& str) {
    char c;
    str = String("");

    while (is.get(c) && std::isspace(c));

    if (is) {
        str.push_back(c);
        while (is.get(c) && !std::isspace(c)) {
            str.push_back(c);
        }
    }
    return is;
}

inline std::ostream& operator<<(std::ostream& os, const String& str) {
    return os << str.c_str();
}

#endif
