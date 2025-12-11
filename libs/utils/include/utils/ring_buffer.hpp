#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

namespace pet {
namespace utils {

// Lock-free Single Producer Single Consumer ring buffer
// Power-of-2 size required for efficient modulo operations
template <typename T, size_t Size>
class RingBuffer {
    static_assert((Size & (Size - 1)) == 0, "Size must be power of 2");
    static_assert(Size > 0, "Size must be greater than 0");

public:
    RingBuffer() : write_pos_(0), read_pos_(0) {}

    // Producer: try to push an element
    // Returns true if successful, false if buffer is full
    [[nodiscard]] bool try_push(const T& item) noexcept {
        const size_t current_write = write_pos_.load(std::memory_order_relaxed);
        const size_t next_write = (current_write + 1) & mask_;
        
        if (next_write == read_pos_.load(std::memory_order_acquire)) {
            return false; // Buffer full
        }
        
        buffer_[current_write] = item;
        write_pos_.store(next_write, std::memory_order_release);
        return true;
    }

    // Producer: push with move semantics
    [[nodiscard]] bool try_push(T&& item) noexcept {
        const size_t current_write = write_pos_.load(std::memory_order_relaxed);
        const size_t next_write = (current_write + 1) & mask_;
        
        if (next_write == read_pos_.load(std::memory_order_acquire)) {
            return false; // Buffer full
        }
        
        buffer_[current_write] = std::move(item);
        write_pos_.store(next_write, std::memory_order_release);
        return true;
    }

    // Consumer: try to pop an element
    // Returns true if successful, false if buffer is empty
    [[nodiscard]] bool try_pop(T& item) noexcept {
        const size_t current_read = read_pos_.load(std::memory_order_relaxed);
        
        if (current_read == write_pos_.load(std::memory_order_acquire)) {
            return false; // Buffer empty
        }
        
        item = std::move(buffer_[current_read]);
        read_pos_.store((current_read + 1) & mask_, std::memory_order_release);
        return true;
    }

    // Check if buffer is empty (consumer side)
    [[nodiscard]] bool empty() const noexcept {
        return read_pos_.load(std::memory_order_acquire) == 
               write_pos_.load(std::memory_order_acquire);
    }

    // Check if buffer is full (producer side)
    [[nodiscard]] bool full() const noexcept {
        const size_t next_write = (write_pos_.load(std::memory_order_relaxed) + 1) & mask_;
        return next_write == read_pos_.load(std::memory_order_acquire);
    }

    // Get approximate size (not exact due to lock-free nature)
    [[nodiscard]] size_t size() const noexcept {
        const size_t write = write_pos_.load(std::memory_order_acquire);
        const size_t read = read_pos_.load(std::memory_order_acquire);
        return (write - read) & mask_;
    }

    [[nodiscard]] static constexpr size_t capacity() noexcept {
        return Size - 1; // One slot reserved to distinguish full from empty
    }

private:
    static constexpr size_t mask_ = Size - 1;
    alignas(64) std::atomic<size_t> write_pos_{0}; // Producer cache line
    alignas(64) std::atomic<size_t> read_pos_{0};  // Consumer cache line
    alignas(64) T buffer_[Size];
};

} // namespace utils
} // namespace pet
