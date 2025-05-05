#include <iostream>
#include <memory>
#include <filesystem>
#include <sstream>
#include <random>
#include <fstream>
#include <string>
#include <algorithm>
#include <queue>
#include <vector>

#include "itape.hpp"

constexpr static const char *USAGE = "Usage: tape-sort <input_file> <output_file>";
constexpr static const int M = 16;
constexpr static int MAX_NUMBERS_COUNT = M / sizeof(int);

static std::string create_file() {
    static std::mt19937 rand(time(nullptr));
    if (!std::filesystem::exists("tmp"))
        std::filesystem::create_directory("tmp");

    std::stringstream filename;
    filename << "tmp/tape_" << rand() << ".tp";
    std::ofstream create(filename.str());
    create.close();
    return filename.str();   
}

static std::unique_ptr<ITape> create_temp_tape() {
    return ITape::new_file_tape(create_file().c_str(), "tape_config.txt");
}

static int read(std::unique_ptr<ITape> &tape) {
    if (tape->current_pos() == tape->size())
        return -1;
    
    int val = tape->read();
    tape->move_forward();
    return val;
}

static void write(std::unique_ptr<ITape> &tape, int val) {
    tape->write(val);
    tape->move_forward();
}

static std::unique_ptr<ITape> merge(std::unique_ptr<ITape> &tape1, std::unique_ptr<ITape> &tape2) {
    auto result = create_temp_tape();
    tape1->rewind(-tape1->size());
    tape2->rewind(-tape2->size());
    int num1 = read(tape1), num2 = read(tape2);
    int ind1 = 1, ind2 = 1;
    while (ind1 <= tape1->size() || ind2 <= tape2->size()) {
        if (ind1 > tape1->size()) {
            write(result, num2);
            num2 = read(tape2);
            ++ind2;
        } else if (ind2 > tape2->size()) {
            write(result, num1);
            num1 = read(tape1);
            ++ind1;
        } else {
            if (num1 <= num2) {
                write(result, num1);
                num1 = read(tape1);
                ++ind1;
            } else {
                write(result, num2);
                num2 = read(tape2);
                ++ind2;
            }
        }
    }
    return std::move(result);
}

static void sort(std::unique_ptr<ITape> &input, std::unique_ptr<ITape> &output) {
    std::queue<std::unique_ptr<ITape>> pieces;
    std::vector<int> nums(MAX_NUMBERS_COUNT);
    while (input->current_pos() < input->size()) {
        int remains = input->size() - input->current_pos();
        int count = std::min(remains, MAX_NUMBERS_COUNT);
        for (int i = 0; i < count; ++i) {
            nums[i] = read(input);
        }
        std::sort(nums.begin(), nums.begin() + count);
        auto tape = create_temp_tape();
        for (int i = 0; i < count; ++i) {
            write(tape, nums[i]);
        }
        pieces.emplace(std::move(tape));
    }
    while (pieces.size() > 1) {
        auto tape1 = std::move(pieces.front());
        pieces.pop();
        auto tape2 = std::move(pieces.front());
        pieces.pop();

        pieces.emplace(merge(tape1, tape2));
    }
    auto result = std::move(pieces.front());
    pieces.pop();
    result->rewind(-result->size());
    for (int i = 0; i < result->size(); ++i) {
        write(output, read(result));
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << USAGE;
        return EXIT_FAILURE;
    }
    std::unique_ptr<ITape> input;
    try {
        input = ITape::new_file_tape(argv[1], "tape_config.txt");
    } catch (std::invalid_argument &e) {
        std::cerr << "Invalid input file " << argv[1];
        return EXIT_FAILURE;
    }
    std::unique_ptr<ITape> output;
    try {
        std::ofstream create(argv[2]);
        create.close();
        output = ITape::new_file_tape(argv[2], "tape_config.txt");
    } catch (std::invalid_argument &e) {
        std::cerr << "Invalid output file " << argv[2];
        return EXIT_FAILURE;
    }
    try {
        sort(input, output);
        std::filesystem::remove_all("tmp");
    } catch (std::filesystem::filesystem_error &e) {
        std::cerr << "Cannot create tmp directory or a file in it \n" << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}