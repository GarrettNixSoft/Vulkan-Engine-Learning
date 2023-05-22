#include "first_app.h"

#include "movement_controller.hpp"
#include "simple_render_system.hpp"
#include "fve_camera.hpp"
#include "fve_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <chrono>
#include <array>

namespace fve {

	struct GlobalUbo {
		glm::mat4 projectionView{ 1.0f };
		glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.0, -3.0f, -1.0f });
	};

	Game::Game() {
		loadGameObjects();
	}

	Game::~Game() {}

	void Game::run() {

		FveBuffer globalUboBuffer{
			device,
			sizeof(GlobalUbo),
			FveSwapChain::MAX_FRAMES_IN_FLIGHT,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			device.properties.limits.minUniformBufferOffsetAlignment
		};
		globalUboBuffer.map();

		SimpleRenderSystem simpleRenderSystem{ device, renderer.getSwapChainRenderPass() };
		FveCamera camera{};
		camera.setViewTarget(glm::vec3(-1, -2, 2), glm::vec3(0.0f, 0.0f, 2.5f));

		auto viewerObject = FveGameObject::createGameObject();
		MovementController cameraController{};
		cameraController.init(window.getGLFWwindow(), WIDTH, HEIGHT);

		auto currentTime = std::chrono::high_resolution_clock::now();

		// game loop
		while (!window.shouldClose()) {
			glfwPollEvents();
			cameraController.update(window.getGLFWwindow());

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveInPlaneXZ(window.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			if (auto commandBuffer = renderer.beginFrame()) {
				// ================ PREPARE ================
				int frameIndex = renderer.getFrameIndex();

				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera
				};

				// ================ INPUT ================
				// camera controls
				float aspect = renderer.getAspectRatio();
				camera.setPerspectiveProjection(glm::radians(cameraController.fov), aspect, 0.1f, 10.0f);

				// ================ UPDATE ================

				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				globalUboBuffer.writeToIndex(&ubo, frameIndex);
				globalUboBuffer.flushIndex(frameIndex);

				// ================ RENDER ================
				
				// begin offscreen shadow pass
				// render shadow casting objects
				// end offscreen shadow pass

				renderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();

			}

		}

		// wait for the GPU to finish whatever it was doing when the user exits the game
		vkDeviceWaitIdle(device.device());

	}

	void Game::loadGameObjects() {
		std::shared_ptr<FveModel> model = FveModel::createModelFromFile(device, "models/flat_vase.obj");

		auto flatVase = FveGameObject::createGameObject();
		flatVase.model = model;
		flatVase.transform.translation = { -0.5f, 0.5f, 2.5f };
		flatVase.transform.scale = { 3.0f, 1.5f, 3.0f };

		gameObjects.push_back(std::move(flatVase));

		model = FveModel::createModelFromFile(device, "models/smooth_vase.obj");

		auto smoothVase = FveGameObject::createGameObject();
		smoothVase.model = model;
		smoothVase.transform.translation = { 0.5f, 0.5f, 2.5f };
		smoothVase.transform.scale = { 3.0f, 1.5f, 3.0f };

		gameObjects.push_back(std::move(smoothVase));
	}

}