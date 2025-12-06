# ============================================================
# PET Project – CMake Toolchain File
# Ensures deterministic builds inside pet-toolchain docker image
# ============================================================
cmake_policy(SET CMP0144 NEW)
# --------------------------------------------
# 1. Force compilers (Clang 18)
# --------------------------------------------
set(CMAKE_C_COMPILER clang-18)
set(CMAKE_CXX_COMPILER clang++-18)

# --------------------------------------------
# 2. Global compile flags
# --------------------------------------------
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fPIC")

# --------------------------------------------
# 3. Where our libraries + headers are installed
# --------------------------------------------
set(PET_DEPS_PREFIX "/opt/deps")

list(APPEND CMAKE_PREFIX_PATH "${PET_DEPS_PREFIX}")
list(APPEND CMAKE_INCLUDE_PATH "${PET_DEPS_PREFIX}/include")
list(APPEND CMAKE_LIBRARY_PATH "${PET_DEPS_PREFIX}/lib")
list(APPEND CMAKE_LIBRARY_PATH "${PET_DEPS_PREFIX}/lib64")

# --------------------------------------------
# 4. Tell CMake we want static libs by default
# --------------------------------------------
set(BUILD_SHARED_LIBS OFF)

# Ensure the linker prefers static libs first
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

# --------------------------------------------
# 5. Configure Boost
# --------------------------------------------
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)

set(BOOST_ROOT "${PET_DEPS_PREFIX}")
set(BOOST_INCLUDEDIR "${PET_DEPS_PREFIX}/include")
set(BOOST_LIBRARYDIR "${PET_DEPS_PREFIX}/lib")

# --------------------------------------------
# 6. Configure OpenSSL
# --------------------------------------------
set(OPENSSL_ROOT_DIR "${PET_DEPS_PREFIX}")
set(OPENSSL_USE_STATIC_LIBS TRUE)

# --------------------------------------------
# 7. Configure simdjson
# --------------------------------------------
set(simdjson_ROOT "${PET_DEPS_PREFIX}")

# --------------------------------------------
# 8. Configure gtest
# --------------------------------------------
set(GTest_ROOT "${PET_DEPS_PREFIX}")

# --------------------------------------------
# 9. Disable tests globally unless user enables them
# --------------------------------------------
option(PET_ENABLE_TESTS "Enable PET tests" OFF)

# --------------------------------------------
# 10. Warn if not building inside docker toolchain
# --------------------------------------------
if (NOT EXISTS "/opt/deps")
    message(WARNING "
===========================
This build environment does NOT contain PET deps!
Make sure you are building inside pet-toolchain:local
===========================
    ")
endif()

