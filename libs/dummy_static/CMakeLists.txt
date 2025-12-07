add_library(dummy_static STATIC
    src/dummy_static.cpp
)

target_include_directories(dummy_static
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Install static library
install(TARGETS dummy_static
    ARCHIVE DESTINATION lib          # static libs: .a
    LIBRARY DESTINATION lib          # not used for static, but safe for future
    RUNTIME DESTINATION bin          # Windows / DLL cases (not used now)
)

# Install public headers
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

