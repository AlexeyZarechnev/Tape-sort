#include <gtest/gtest.h>
#include <stdexcept>

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

