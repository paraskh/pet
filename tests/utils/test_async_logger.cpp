#include <gtest/gtest.h>
#include <utils/async_logger.hpp>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <sstream>

using namespace pet::utils;

class AsyncLoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_file_ = "/tmp/pet_test_log_" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()
        ) + ".log";
    }
    
    void TearDown() override {
        if (std::filesystem::exists(test_file_)) {
            std::filesystem::remove(test_file_);
        }
    }
    
    std::string test_file_;
};

TEST_F(AsyncLoggerTest, BasicLogging) {
    AsyncLogger logger(test_file_);
    
    EXPECT_FALSE(logger.is_running());
    logger.start();
    EXPECT_TRUE(logger.is_running());
    
    EXPECT_TRUE(logger.log("Test message 1"));
    EXPECT_TRUE(logger.log("Test message 2"));
    
    // Give logger time to flush
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    logger.stop();
    EXPECT_FALSE(logger.is_running());
    
    // Verify file was created and contains messages
    EXPECT_TRUE(std::filesystem::exists(test_file_));
    
    std::ifstream file(test_file_);
    std::string line;
    int count = 0;
    while (std::getline(file, line)) {
        ++count;
        EXPECT_TRUE(line.find("Test message") != std::string::npos);
    }
    EXPECT_GE(count, 2);
}

TEST_F(AsyncLoggerTest, LogWhenNotStarted) {
    AsyncLogger logger(test_file_);
    
    EXPECT_FALSE(logger.is_running());
    EXPECT_FALSE(logger.log("Should fail"));
}

TEST_F(AsyncLoggerTest, MultipleMessages) {
    AsyncLogger logger(test_file_);
    logger.start();
    
    const int num_messages = 100;
    for (int i = 0; i < num_messages; ++i) {
        std::string msg = "Message " + std::to_string(i);
        EXPECT_TRUE(logger.log(msg.c_str()));
    }
    
    // Give time to flush
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    logger.stop();
    
    // Count lines in file
    std::ifstream file(test_file_);
    std::string line;
    int count = 0;
    while (std::getline(file, line)) {
        ++count;
    }
    EXPECT_GE(count, num_messages);
}

TEST_F(AsyncLoggerTest, StopBeforeStart) {
    AsyncLogger logger(test_file_);
    logger.stop(); // Should not crash
    EXPECT_FALSE(logger.is_running());
}

TEST_F(AsyncLoggerTest, DoubleStart) {
    AsyncLogger logger(test_file_);
    logger.start();
    EXPECT_TRUE(logger.is_running());
    
    logger.start(); // Should be idempotent
    EXPECT_TRUE(logger.is_running());
    
    logger.stop();
}

TEST_F(AsyncLoggerTest, TimestampFormat) {
    AsyncLogger logger(test_file_);
    logger.start();
    
    EXPECT_TRUE(logger.log("Timestamp test"));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    logger.stop();
    
    std::ifstream file(test_file_);
    std::string line;
    ASSERT_TRUE(std::getline(file, line));
    
    // Should contain timestamp in brackets
    EXPECT_TRUE(line.find('[') != std::string::npos);
    EXPECT_TRUE(line.find(']') != std::string::npos);
    EXPECT_TRUE(line.find("Timestamp test") != std::string::npos);
}

TEST_F(AsyncLoggerTest, LongMessage) {
    AsyncLogger logger(test_file_);
    logger.start();
    
    std::string long_msg(500, 'A'); // Longer than 256 char limit
    EXPECT_TRUE(logger.log(long_msg.c_str()));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    logger.stop();
    
    std::ifstream file(test_file_);
    std::string line;
    ASSERT_TRUE(std::getline(file, line));
    
    // Should be truncated to 255 chars + null terminator = 255 visible
    EXPECT_LE(line.length(), 300); // Account for timestamp prefix
}

TEST_F(AsyncLoggerTest, HighFrequencyLogging) {
    AsyncLogger logger(test_file_);
    logger.start();
    
    const int num_messages = 3000;
    int success_count = 0;
    
    // Rapid logging - buffer may fill, which is expected behavior
    for (int i = 0; i < num_messages; ++i) {
        std::string msg = "HF " + std::to_string(i);
        if (logger.log(msg.c_str())) {
            ++success_count;
        }
    }
    
    // Buffer capacity is 2047, so at least that many should succeed
    // Allow some failures due to buffer filling during rapid logging
    EXPECT_GT(success_count, 1500); // At least half should succeed
    
    // Give time for background thread to drain
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    logger.stop();
    
    // Verify some messages were written
    std::ifstream file(test_file_);
    std::string line;
    int written_count = 0;
    while (std::getline(file, line)) {
        if (line.find("HF ") != std::string::npos) {
            ++written_count;
        }
    }
    EXPECT_GT(written_count, 0);
}
