#include "first_app.hpp"

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
		alignas(16) glm::mat4 projectionView{ 1.0f };
		//alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.0, -3.0f, -1.0f });
		alignas(16) glm::vec4 ambientLightColor{1.0f, 1.0f, 1.0f, 0.02f}; // w is light intensity
		alignas(16) glm::vec3 lightPosition{ -1.0f };
		alignas(16) glm::vec4 lightColor{ 1.0f }; // w is light intensity

	};

	Game::Game() {
		globalPool = FveDescriptorPool::Builder(device)
			.setMaxSets(FveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, FveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	Game::~Game() {}

	void Game::run() {

		std::vector<std::unique_ptr<FveBuffer>> uboBuffers(FveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<FveBuffer>(
				device,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				device.properties.limits.minUniformBufferOffsetAlignment);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = FveDescriptorSetLayout::Builder(device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriprorSets(FveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriprorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			FveDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriprorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		FveCamera camera{};
		camera.setViewTarget(glm::vec3(-1, -2, 2), glm::vec3(0.0f, 0.0f, 2.5f));

		auto viewerObject = FveGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
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
					camera,
					globalDescriprorSets[frameIndex],
					gameObjects
				};

				// ================ INPUT ================
				// camera controls
				float aspect = renderer.getAspectRatio();
				camera.setPerspectiveProjection(glm::radians(cameraController.fov), aspect, 0.1f, 1000.0f);

				// ================ UPDATE ================

				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// ================ RENDER ================
				
				// begin offscreen shadow pass
				// render shadow casting objects
				// end offscreen shadow pass

				renderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo);
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
		flatVase.transform.translation = { -0.5f, 0.5f, 0.0f };
		flatVase.transform.scale = { 3.0f, 1.5f, 3.0f };

		gameObjects.emplace(flatVase.getId(), std::move(flatVase));

		model = FveModel::createModelFromFile(device, "models/smooth_vase.obj");

		auto smoothVase = FveGameObject::createGameObject();
		smoothVase.model = model;
		smoothVase.transform.translation = { 0.5f, 0.5f, 0.0f };
		smoothVase.transform.scale = { 3.0f, 1.5f, 3.0f };

		gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

		model = FveModel::createModelFromFile(device, "models/quad.obj");
		auto floor = FveGameObject::createGameObject();
		floor.model = model;
		floor.transform.translation = {0.0f, 0.5f, 0.0f};
		floor.transform.scale = { 3.0f, 1.0f, 3.0f };
		gameObjects.emplace(floor.getId(), std::move(floor));
	}

}