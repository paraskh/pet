# ---------------------------------------
# Foo library
# ---------------------------------------

find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(OpenSSL REQUIRED)
find_package(simdjson REQUIRED)

add_library(foo STATIC
    foo.cpp
)

target_include_directories(foo PUBLIC
    ${Boost_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(foo PUBLIC
    Boost::filesystem
    OpenSSL::SSL
    OpenSSL::Crypto
    simdjson::simdjson
)

target_compile_features(foo PUBLIC cxx_std_20)

