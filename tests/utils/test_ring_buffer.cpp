#include <gtest/gtest.h>
#include <utils/ring_buffer.hpp>
#include <thread>
#include <vector>

using namespace pet::utils;

TEST(RingBufferTest, BasicPushPop) {
    RingBuffer<int, 16> buffer;
    
    EXPECT_TRUE(buffer.empty());
    EXPECT_FALSE(buffer.full());
    
    int value = 42;
    EXPECT_TRUE(buffer.try_push(value));
    EXPECT_FALSE(buffer.empty());
    
    int result = 0;
    EXPECT_TRUE(buffer.try_pop(result));
    EXPECT_EQ(result, 42);
    EXPECT_TRUE(buffer.empty());
}

TEST(RingBufferTest, Capacity) {
    RingBuffer<int, 16> buffer;
    EXPECT_EQ(buffer.capacity(), 15); // Size - 1
    
    RingBuffer<int, 64> buffer64;
    EXPECT_EQ(buffer64.capacity(), 63);
}

TEST(RingBufferTest, FullBuffer) {
    RingBuffer<int, 16> buffer;
    
    // Fill buffer to capacity
    for (int i = 0; i < 15; ++i) {
        EXPECT_TRUE(buffer.try_push(i));
    }
    
    EXPECT_TRUE(buffer.full());
    EXPECT_FALSE(buffer.try_push(999)); // Should fail
    
    // Pop one
    int val;
    EXPECT_TRUE(buffer.try_pop(val));
    EXPECT_FALSE(buffer.full());
    
    // Now can push again
    EXPECT_TRUE(buffer.try_push(999));
}

TEST(RingBufferTest, MoveSemantics) {
    RingBuffer<std::string, 16> buffer;
    
    std::string msg = "test message";
    EXPECT_TRUE(buffer.try_push(std::move(msg)));
    EXPECT_TRUE(msg.empty()); // Moved from
    
    std::string result;
    EXPECT_TRUE(buffer.try_pop(result));
    EXPECT_EQ(result, "test message");
}

TEST(RingBufferTest, MultiplePushPop) {
    RingBuffer<int, 16> buffer;
    
    // Push multiple
    for (int i = 0; i < 10; ++i) {
        EXPECT_TRUE(buffer.try_push(i));
    }
    
    // Pop multiple
    for (int i = 0; i < 10; ++i) {
        int val;
        EXPECT_TRUE(buffer.try_pop(val));
        EXPECT_EQ(val, i);
    }
    
    EXPECT_TRUE(buffer.empty());
}

TEST(RingBufferTest, SizeCalculation) {
    RingBuffer<int, 16> buffer;
    
    EXPECT_EQ(buffer.size(), 0);
    
    buffer.try_push(1);
    EXPECT_EQ(buffer.size(), 1);
    
    buffer.try_push(2);
    EXPECT_EQ(buffer.size(), 2);
    
    int val;
    buffer.try_pop(val);
    EXPECT_EQ(buffer.size(), 1);
}

TEST(RingBufferTest, SPSCThreaded) {
    RingBuffer<int, 1024> buffer;
    constexpr int num_items = 10000;
    std::vector<int> received;
    received.reserve(num_items);
    
    std::atomic<bool> producer_done{false};
    
    // Producer thread
    std::thread producer([&]() {
        for (int i = 0; i < num_items; ++i) {
            while (!buffer.try_push(i)) {
                std::this_thread::yield();
            }
        }
        producer_done = true;
    });
    
    // Consumer thread
    std::thread consumer([&]() {
        int val;
        while (!producer_done.load() || !buffer.empty()) {
            if (buffer.try_pop(val)) {
                received.push_back(val);
            } else {
                std::this_thread::yield();
            }
        }
    });
    
    producer.join();
    consumer.join();
    
    EXPECT_EQ(received.size(), num_items);
    
    // Verify order (should be mostly preserved, though not guaranteed)
    for (size_t i = 0; i < received.size(); ++i) {
        EXPECT_EQ(received[i], static_cast<int>(i));
    }
}
