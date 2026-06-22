#include "string.hpp"

String::String() : data(new char[1]{'\0'}), len(0), capacity(0) {}

String::String(const char* s) {
    len = std::strlen(s);
    capacity = len;
    data = new char[len + 1];
    std::memcpy(data, s, len + 1);
}

String::~String() { delete[] data; }

String::String(size_t n, char c) {
    len = n;
    capacity = n;
    data = new char[len + 1];
    std::memset(data, c, len);
    data[len] = '\0';
}

String::String(const String& other) : len(other.len), capacity(other.len) {
    data = new char[len + 1];
    std::memcpy(data, other.data, len + 1);
}

char* String::begin() { return data; }
char* String::end() { return data + len; }

const char* String::begin() const { return data; }
const char* String::end() const { return data + len; }

String::operator std::string() const { return std::string(data); }

String::operator std::filesystem::path() const { return std::filesystem::path(data); }

String& String::operator=(const String& other) {
    if (this != &other) {
        delete[] data;
        len = other.len;
        capacity = other.len;
        data = new char[len + 1];
        std::memcpy(data, other.data, len + 1);
    }
    return *this;
}

bool String::operator==(const char* other) { return std::strcmp(data, other) == 0; }

bool String::operator!=(const char* other) { return std::strcmp(data, other) != 0; }

size_t String::length() const { return len; }

bool String::empty() const { return len == 0; }

char& String::operator[](size_t i) { return data[i]; }

const char& String::operator[](size_t i) const { return data[i]; }

void String::push_back(char c) {
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

void String::append(size_t count, char c) {
    for (size_t i = 0; i < count; ++i) push_back(c);
}

String String::operator+(const std::string& s) {
    String result = *this;
    for (char c : s) result.push_back(c);
    return result;
}

String& String::operator+=(const std::string& s) {
    for (char c : s) push_back(c);
    return *this;
}

const char* String::c_str() const { return data; }
