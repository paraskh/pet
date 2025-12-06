#include "foo.hpp"

// Boost
#include <boost/filesystem.hpp>

// OpenSSL
#include <openssl/sha.h>

// simdjson
#include <simdjson.h>

#include <fstream>
#include <sstream>
#include <iomanip>

namespace pet::foo {

// Convert byte array → hex string
static std::string to_hex(const unsigned char* data, std::size_t len) {
    std::ostringstream oss;
    for (size_t i = 0; i < len; i++) {
        oss << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(data[i]);
    }
    return oss.str();
}

FooResult process(const std::string& path, const std::string& json_input) {

    FooResult result{};

    // ------------------------------
    // 1. BOOST: check if file exists
    // ------------------------------
    boost::filesystem::path p(path);
    result.path_exists = boost::filesystem::exists(p);

    // ------------------------------
    // 2. OPENSSL: SHA256 hash of json_input
    // ------------------------------
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(json_input.data()),
           json_input.size(), hash);

    result.sha256_hex = to_hex(hash, SHA256_DIGEST_LENGTH);

    // ------------------------------
    // 3. SIMDJSON: parse JSON & extract a field "value"
    // ------------------------------
    simdjson::dom::parser parser;
    simdjson::dom::element elem;

    auto error = parser.parse(json_input).get(elem);
    if (error == simdjson::SUCCESS) {
        // if JSON contains {"value": "something"}
        auto value = elem["value"];
        if (value.error() == simdjson::SUCCESS) {
            result.json_value = std::string(std::string_view(value));
        } else {
            result.json_value = "<no value>";
        }
    } else {
        result.json_value = "<invalid json>";
    }

    return result;
}

} // namespace pet::foo

