add_library(dummy_shared SHARED
    src/dummy_shared.cpp
)

target_include_directories(dummy_shared
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Allow RPATH for .so to be embedded (important for releases!)
set_target_properties(dummy_shared PROPERTIES
    INSTALL_RPATH "$ORIGIN/../lib"
    BUILD_WITH_INSTALL_RPATH TRUE
)

# Install the library (.so)
install(TARGETS dummy_shared
    LIBRARY DESTINATION lib          # .so files
    ARCHIVE DESTINATION lib          # if someone builds static later
    RUNTIME DESTINATION bin          # for Windows .dll
)

# Install headers
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)
