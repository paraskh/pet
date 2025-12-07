add_library(dummy_shared SHARED
    src/dummy_shared.cpp
)

target_include_directories(dummy_shared
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Install shared lib + headers
install(TARGETS dummy_shared
        LIBRARY DESTINATION lib)

install(DIRECTORY include/
        DESTINATION include)

