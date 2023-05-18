#include "first_app.h"

namespace fve {

	void Game::run() {

		while (!window.shouldClose()) {
			glfwPollEvents();
		}

	}

}