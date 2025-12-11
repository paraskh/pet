# Static library with both source files and header-only components
add_library(utils STATIC
    src/utils.cpp
)

target_include_directories(utils PUBLIC include)
