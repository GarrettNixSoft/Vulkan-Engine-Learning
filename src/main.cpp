#include "game.hpp"
#include "fve_window.hpp"
#include "fve_device.hpp"
#include "fve_memory.hpp"
#include "fve_constants.hpp"
#include "fve_globals.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <cassert>

void runGame() {

    fve::FveWindow window{ fve::WIDTH, fve::HEIGHT, "First Vulkan Game" };
    fve::FveDevice device{ window };

    {
        fve::Game game{ window, device };
        game.run();
    }

    vmaDestroyAllocator(fve::fveAllocator);

}

int main() {

    try {
        runGame();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    extern int BUFFER_ALLOCATIONS;
    assert(BUFFER_ALLOCATIONS == 0 && "Buffer alloc/dealloc mismatch!");
    if (BUFFER_ALLOCATIONS == 0) std::cout << "Buffer alloc/dealloc seems to match" << std::endl;

    return EXIT_SUCCESS;
}