#include <gtest/gtest.h>
#include <stdexcept>
#include <chrono>
#include <memory>
#include <functional>
#include <fstream>
#include <filesystem>

#include "itape.hpp"

TEST(Creation, normalCreationTest) {
    ASSERT_NO_THROW(auto tape = ITape::new_file_tape("data/empty.tp", "conf/fast.txt"));
}

TEST(Creation, invalidData) {
    ASSERT_THROW(auto tape = ITape::new_file_tape("invalid", "conf/fast.txt"), std::invalid_argument);
}

TEST(Creation, invalidConf) {
    ASSERT_THROW(auto tape = ITape::new_file_tape("data/empty.tp", "invalid"), std::invalid_argument);
}

TEST(Creation, confWithInvalidParameters) {
    ASSERT_THROW(auto tape = ITape::new_file_tape("data/empty.tp", "conf/illegal.txt"), std::invalid_argument);
}

TEST(Reading, emptyRead) {
    auto tape = ITape::new_file_tape("data/empty.tp", "conf/fast.txt");
    ASSERT_THROW(tape->read(), std::out_of_range);
}

TEST(Reading, oneRead) {
    auto tape = ITape::new_file_tape("data/one_number.tp", "conf/fast.txt");
    int num;
    ASSERT_NO_THROW(num = tape->read());
    EXPECT_EQ(12306, num);
}

TEST(Reading, multipleRead) {
    auto tape = ITape::new_file_tape("data/four_numbers.tp", "conf/fast.txt");
    int nums[4];
    ASSERT_NO_THROW(
        for (int i = 0; i < 4; ++i) {
            nums[i] = tape->read();
            tape->move_forward();
        }
    );
    EXPECT_EQ(24, nums[0]);
    EXPECT_EQ(54, nums[1]);
    EXPECT_EQ(8452, nums[2]);
    EXPECT_EQ(4098, nums[3]);
}

void test_delay(std::function<void(std::unique_ptr<ITape>&)> action, std::unique_ptr<ITape>&& tape, int expected_delay) {
    auto start_time = std::chrono::high_resolution_clock::now();
    action(tape);
    auto end_time = std::chrono::high_resolution_clock::now();
    EXPECT_LE(std::chrono::milliseconds(expected_delay), std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time)) << \
        "expected delay: " << expected_delay << " real delay:" << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
}

void test_delay(std::function<void(std::unique_ptr<ITape>&)> action, int expected_delay) {
    test_delay(action, ITape::new_file_tape("data/four_numbers.tp", "conf/slow.txt"), expected_delay);
}

TEST(Reading, delayTest) {
    test_delay([](std::unique_ptr<ITape> &tape) { tape->read(); }, 150);
}

TEST(Move, oneMoveTest) {
    auto tape = ITape::new_file_tape("data/four_numbers.tp", "conf/fast.txt");
    EXPECT_EQ(0, tape->current_pos());
    tape->move_forward();
    EXPECT_EQ(1, tape->current_pos());
}

TEST(Move, multipleMoveTest) {
    auto tape = ITape::new_file_tape("data/four_numbers.tp", "conf/fast.txt");
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(i, tape->current_pos());
        tape->move_forward();
    }
    tape->move_back();
    EXPECT_EQ(3, tape->current_pos());
    tape->move_back();
    EXPECT_EQ(2, tape->current_pos());
}

TEST(Move, delayTest) {
    test_delay([](std::unique_ptr<ITape> &tape) { tape->move_forward(); }, 500);
}

TEST(Rewind, normalTest) {
    auto tape = ITape::new_file_tape("data/four_numbers.tp", "conf/fast.txt");
    ASSERT_NO_THROW(tape->rewind(2));
}

TEST(Rewind, invalidTest) {
    auto tape = ITape::new_file_tape("data/four_numbers.tp", "conf/fast.txt");
    ASSERT_THROW(tape->rewind(10), std::invalid_argument);
}

TEST(Rewind, delayTest) {
    test_delay([](std::unique_ptr<ITape> &tape) { tape->rewind(2); }, 1500);
}

class Write : public testing::Test {
protected:
    std::unique_ptr<ITape> tape;

    void SetUp() override {
        std::ofstream tape_file("test_tape.tp");
        tape_file.close();
        tape = ITape::new_file_tape("test_tape.tp", "conf/middle.txt");
    }
   
    void TearDown() override {
        std::filesystem::remove(std::filesystem::path("test_tape.tp"));
    }
};

TEST_F(Write, writeToEmptyTapeTest) {
    tape->write(2048);
    EXPECT_EQ(1, tape->size());
    EXPECT_EQ(2048, tape->read());
}

TEST_F(Write, rewriteTest) {
    tape->write(2048);
    tape->write(33);
    EXPECT_EQ(1, tape->size());
    EXPECT_EQ(33, tape->read());
}

TEST_F(Write, multipleWriteTest) {
    for (int i = 0; i < 3; ++i) {
        tape->write(2304);
        tape->move_forward();
    }
    EXPECT_EQ(3, tape->size());
}

TEST_F(Write, delayTest) {
    test_delay([](std::unique_ptr<ITape> &tape) { tape->write(2048); }, std::move(tape), 240);
}