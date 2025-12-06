cmake_minimum_required(VERSION 3.20)
if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)
endif()
project(pet LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirs
add_subdirectory(libs)
add_subdirectory(apps)

if (PET_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

