#pragma once

#include "fve_window.hpp"
#include "fve_device.hpp"
#include "fve_renderer.hpp"
#include "fve_game_object.hpp"

#include <memory>
#include <vector>

namespace fve {

	class Game {
	public:
		static constexpr int WIDTH = 1920;
		static constexpr int HEIGHT = 1080;

		Game();
		~Game();

		void run();
	private:
		FveWindow window{ WIDTH, HEIGHT, "First Vulkan Game" };
		FveDevice device{ window };
		FveRenderer renderer{window, device};

		std::vector<FveGameObject> gameObjects;

		Game(const Game&) = delete;
		Game& operator=(const Game&) = delete;

		void loadGameObjects();
	};

}