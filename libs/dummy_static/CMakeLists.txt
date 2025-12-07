add_library(dummy_static STATIC
    src/dummy_static.cpp
)

target_include_directories(dummy_static
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Install static lib + headers
install(TARGETS dummy_static
        ARCHIVE DESTINATION lib)

install(DIRECTORY include/
        DESTINATION include)

