cmake_minimum_required(VERSION 3.20)
if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)
endif()
project(pet LANGUAGES C CXX)

option(PET_ENABLE_SANITIZERS "Enable Address/UB sanitizers" OFF)

if(PET_ENABLE_SANITIZERS)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)

# Global OpenSSL hints for static toolchain
set(OPENSSL_ROOT_DIR "/opt/deps")
set(OPENSSL_USE_STATIC_LIBS TRUE)
set(OPENSSL_INCLUDE_DIR "/opt/deps/include")
set(OPENSSL_CRYPTO_LIBRARY "/opt/deps/lib64/libcrypto.a")
set(OPENSSL_SSL_LIBRARY "/opt/deps/lib64/libssl.a")


# Make all targets position independent (good for shared libs)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Runtime search path for installed binaries: bin/ looks into ../lib
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirs
add_subdirectory(libs)
add_subdirectory(apps)

if (PET_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

