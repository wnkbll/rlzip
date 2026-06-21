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
    String() : data(new char[1]{'\0'}), len(0), capacity(0) {}

    String(const char* s) {
        len = std::strlen(s);
        capacity = len;
        data = new char[len + 1];
        std::memcpy(data, s, len + 1);
    }

    ~String() { delete[] data; }

    String(size_t n, char c) {
        len = n;
        capacity = n;
        data = new char[len + 1];
        std::memset(data, c, len);
        data[len] = '\0';
    }

    String(const String& other) : len(other.len), capacity(other.len) {
        data = new char[len + 1];
        std::memcpy(data, other.data, len + 1);
    }

    char* begin() { return data; }
    char* end() { return data + len; }

    const char* begin() const { return data; }
    const char* end() const { return data + len; }

    operator std::string() const { return std::string(data); }

    operator std::filesystem::path() const { return std::filesystem::path(data); }

    String& operator=(const String& other) {
        if (this != &other) {
            delete[] data;
            len = other.len;
            capacity = other.len;
            data = new char[len + 1];
            std::memcpy(data, other.data, len + 1);
        }
        return *this;
    }

    bool operator==(const char* other) { return std::strcmp(data, other) == 0; }

    bool operator!=(const char* other) { return std::strcmp(data, other) != 0; }

    size_t length() const { return len; }

    bool empty() const { return len == 0; }

    char& operator[](size_t i) { return data[i]; }

    const char& operator[](size_t i) const { return data[i]; }

    void push_back(char c) {
        if (len + 1 > capacity) {
            capacity = (capacity == 0) ? 16 : capacity * 2;
            char* newData = new char[capacity + 1];
            std::memcpy(newData, data, len);
            delete[] data;
            data = newData;
        }
        data[len++] = c;
        data[len] = '\0';
    }

    void append(size_t count, char c) {
        for (size_t i = 0; i < count; ++i) push_back(c);
    }

    String operator+(const std::string& s) {
        String result = *this;
        for (char c : s) result.push_back(c);
        return result;
    }

    String& operator+=(const std::string& s) {
        for (char c : s) push_back(c);
        return *this;
    }

    const char* c_str() const { return data; }
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
