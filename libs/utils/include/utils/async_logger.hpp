#pragma once

#include "ring_buffer.hpp"
#include <atomic>
#include <chrono>
#include <cstring>
#include <fstream>
#include <string>
#include <thread>

namespace pet {
namespace utils {

// Async buffer logger for high-performance logging
// Uses lock-free ring buffer to minimize latency
class AsyncLogger {
public:
    struct LogEntry {
        char message[256];
        std::chrono::nanoseconds timestamp;
    };

    explicit AsyncLogger(const std::string& filepath)
        : filepath_(filepath)
        , running_(false)
        , thread_() {
    }

    ~AsyncLogger() {
        stop();
    }

    // Start the background logging thread
    void start() {
        if (running_.exchange(true)) {
            return; // Already running
        }
        
        file_.open(filepath_, std::ios::out | std::ios::app);
        if (!file_.is_open()) {
            running_ = false;
            return;
        }
        
        thread_ = std::thread(&AsyncLogger::flush_loop, this);
    }

    // Stop the background logging thread
    void stop() {
        if (!running_.exchange(false)) {
            return; // Already stopped
        }
        
        if (thread_.joinable()) {
            thread_.join();
        }
        
        // Flush remaining entries
        flush_remaining();
        
        if (file_.is_open()) {
            file_.close();
        }
    }

    // Log a message (non-blocking, returns false if buffer is full)
    [[nodiscard]] bool log(const char* msg) noexcept {
        if (!running_.load(std::memory_order_acquire)) {
            return false;
        }

        LogEntry entry;
        const size_t msg_len = std::strlen(msg);
        const size_t copy_len = (msg_len < sizeof(entry.message) - 1) 
                                ? msg_len 
                                : sizeof(entry.message) - 1;
        
        std::memcpy(entry.message, msg, copy_len);
        entry.message[copy_len] = '\0';
        entry.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        );

        return buffer_.try_push(std::move(entry));
    }

    // Check if logger is running
    [[nodiscard]] bool is_running() const noexcept {
        return running_.load(std::memory_order_acquire);
    }

private:
    void flush_loop() {
        LogEntry entry;
        constexpr auto flush_interval = std::chrono::milliseconds(10);
        
        while (running_.load(std::memory_order_acquire)) {
            bool flushed_any = false;
            
            // Drain buffer
            while (buffer_.try_pop(entry)) {
                write_entry(entry);
                flushed_any = true;
            }
            
            if (flushed_any && file_.is_open()) {
                file_.flush();
            }
            
            std::this_thread::sleep_for(flush_interval);
        }
    }

    void flush_remaining() {
        LogEntry entry;
        while (buffer_.try_pop(entry)) {
            write_entry(entry);
        }
        if (file_.is_open()) {
            file_.flush();
        }
    }

    void write_entry(const LogEntry& entry) {
        if (!file_.is_open()) {
            return;
        }
        
        file_ << "[" << entry.timestamp.count() << "] " 
              << entry.message << "\n";
    }

    std::string filepath_;
    RingBuffer<LogEntry, 2048> buffer_; // 2K entries (power of 2) - ~528KB total
    std::atomic<bool> running_;
    std::thread thread_;
    std::ofstream file_;
};

} // namespace utils
} // namespace pet
