#pragma once

#include <string>
#include <vector>

namespace pet::foo {

struct FooResult {
    bool path_exists;
    std::string sha256_hex;
    std::string json_value;
};

FooResult process(const std::string& path, const std::string& json_input);

} // namespace pet::foo

