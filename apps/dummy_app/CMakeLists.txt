add_executable(dummy_app
    src/main.cpp
)

target_link_libraries(dummy_app
    PRIVATE
        dummy_static
        dummy_shared
)

# Ensure app sees headers of linked libraries
target_include_directories(dummy_app
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../dummy_static/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../dummy_shared/include
)

# Install final executable
install(TARGETS dummy_app
    RUNTIME DESTINATION bin       # Linux + macOS
    LIBRARY DESTINATION bin       # In case it’s treated as a bundle (macOS)
    ARCHIVE DESTINATION bin       # Rare, but future-proof
)

