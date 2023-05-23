#include "first_app.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {

    fve::Game game{};

    try {
        game.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}