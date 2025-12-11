# Find GTest
find_package(GTest REQUIRED)

# Test executables
add_executable(test_ring_buffer
    utils/test_ring_buffer.cpp
)

add_executable(test_async_logger
    utils/test_async_logger.cpp
)

# Link libraries
target_link_libraries(test_ring_buffer
    PRIVATE
    utils
    GTest::gtest
    GTest::gtest_main
)

target_link_libraries(test_async_logger
    PRIVATE
    utils
    GTest::gtest
    GTest::gtest_main
)

# Add tests
add_test(NAME RingBufferTest COMMAND test_ring_buffer)
add_test(NAME AsyncLoggerTest COMMAND test_async_logger)
