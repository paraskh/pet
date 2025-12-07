add_executable(dummy_app
    src/main.cpp
)

target_link_libraries(dummy_app
    PRIVATE
        dummy_static
        dummy_shared
)

# Install app to bin/
install(TARGETS dummy_app
        RUNTIME DESTINATION bin)

