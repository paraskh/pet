#include <iostream>

#include "dummy_static/dummy_static.hpp"
#include "dummy_shared/dummy_shared.hpp"

int main() {
    std::cout << "dummy_app starting..." << std::endl;
    std::cout << dummy_static::hello() << std::endl;
    std::cout << dummy_shared::info() << std::endl;
    return 0;
}

