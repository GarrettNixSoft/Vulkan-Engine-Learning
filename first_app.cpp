#include "first_app.h"
#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>
#include <stdexcept>
#include <array>
#include <cassert>

namespace fve {

	Game::Game() {
		loadGameObjects();
	}

	Game::~Game() {}

	void Game::run() {

		SimpleRenderSystem simpleRenderSystem{ device, renderer.getSwapChainRenderPass() };

		// game loop
		while (!window.shouldClose()) {
			glfwPollEvents();
			
			if (auto commandBuffer = renderer.beginFrame()) {

				// begin offscreen shadow pass
				// render shadow casting objects
				// end offscreen shadow pass

				renderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();

			}

		}

		// wait for the GPU to finish whatever it was doing when the user exits the game
		vkDeviceWaitIdle(device.device());

	}

	void Game::loadGameObjects() {
		std::vector<FveModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		std::vector<glm::vec3> colors{
			{1.f, .7f, .73f},
			{1.f, .87f, .73f},
			{1.f, 1.f, .73f},
			{.73f, 1.f, .8f},
			{.73, .88f, 1.f}
		};

		auto model = std::make_shared<FveModel>(device, vertices);

		for (int i = 0; i < 40; i++) {
			auto triangle = FveGameObject::createGameObject();
			triangle.model = model;
			triangle.color = colors[i % colors.size()];
			triangle.transform2d.scale = glm::vec2(0.5) + i * 0.025f;
			triangle.transform2d.rotation = i * glm::two_pi<float>() + 0.025f;

			gameObjects.push_back(std::move(triangle));
		}
	}

}