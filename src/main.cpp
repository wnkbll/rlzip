#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <vector>

#include "encoder.hpp"

bool is_valid_path(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        return false;
    }

    if (std::filesystem::is_directory(path)) {
        return false;
    }

    return true;
}

void print_help() {
    std::print("Использование: rlzip <command> <stream> [-o <путь>]\n\n");
    std::print(
        "Утилита для кодирования и декодирования методом RLE (Run-Length "
        "Encoding).\n\n");
    std::print("Обязательные аргументы:\n");
    std::print(
        "  <command>       Режим работы: 'encode' (кодировать) или 'decode' "
        "(декодировать)\n");
    std::print(
        "  <stream>         Входная строка текста ИЛИ путь к входному файлу\n\n");
    std::print("Опциональные аргументы:\n");
    std::print(
        "  -o <путь>       Путь к выходному файлу (если не указан, вывод производится "
        "в консоль)\n\n");
    std::print("Примеры:\n");
    std::print("  rlzip encode \"AAAAABBB\" -o output.txt\n");
    std::print("  rlzip decode input.txt\n");
}

int main(int argc, char** argv) {
    std::vector<std::string> args(argv, argv + argc);

    if (argc < 3 || args[1] == "-h" || args[1] == "--help") {
        print_help();
        return (argc < 3) ? 1 : 0;
    }

    bool is_path_stream = true;
    std::string command = args[1];
    std::string stream = args[2];
    std::string output_path = "";

    if (command != "encode" && command != "decode") {
        std::println(
            std::cerr,
            "Ошибка: Неверная команда '{}'. Ожидается 'encode' или 'decode'.\n",
            command);
        print_help();
        return 1;
    }

    if (!is_valid_path(stream)) {
        is_path_stream = false;
    }

    for (size_t i = 3; i < args.size(); ++i) {
        if (args[i] == "-o") {
            if (i + 1 < args.size()) {
                output_path = args[i + 1];
                i++;
            } else {
                std::println(std::cerr,
                             "Ошибка: Для опции -o необходимо указать путь к файлу.\n");
                print_help();
                return 1;
            }
        } else {
            std::println(std::cerr, "Ошибка: Неизвестный аргумент '{}'\n", args[i]);
            print_help();
            return 1;
        }
    }

    std::string input;
    if (is_path_stream) {
        std::ifstream file(stream);
        file >> input;
        file.close();
    } else {
        input = stream;
    }

    std::string result;

    if (command == "encode") {
        result = encoder::encode(input);
    } else if (command == "decode") {
        result = encoder::decode(input);
    }

    if (output_path != "") {
        std::ofstream file(output_path);
        file << result;
    } else {
        std::println("{}", result);
    }

    return 0;
}
