#include "game.hpp"

#include "movement_controller.hpp"
#include "systems/simple_render_system.hpp"
#include "systems/point_light_system.hpp"
#include "systems/textured_render_system.hpp"
#include "fve_camera.hpp"
#include "fve_buffer.hpp"
#include "fve_memory.hpp"
#include "fve_assets.hpp"
#include "fve_initializers.hpp"
#include "fve_constants.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <chrono>
#include <array>

namespace fve {

	Game::Game(FveWindow& window, FveDevice& device) : window{ window }, device{ device } {

		const int numSystems = 2;

		globalPool = FveDescriptorPool::Builder(device)
			.setMaxSets(FveSwapChain::MAX_FRAMES_IN_FLIGHT * numSystems)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, FveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, FveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		globalSetLayout = FveDescriptorSetLayout::Builder(device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();
		texturedSetLayout = FveDescriptorSetLayout::Builder(device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();
	}

	Game::~Game() {
		fveAssets.cleanUp(device);
	}

	void Game::init() {
		// TODO
	}

	void Game::run() {

		std::vector<std::unique_ptr<FveBuffer>> uboBuffers(FveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<FveBuffer>(
				fveAllocator,
				device,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VMA_MEMORY_USAGE_CPU_TO_GPU,
				"uboBuffers",
				device.properties.limits.minUniformBufferOffsetAlignment);
			uboBuffers[i]->map();
		}

		// used to init globalSetLayout here

		// ================ PREPARE ASSETS ================
		loadTextures();

		// ================ PREPARE RENDERING SYSTEMS ================
		SimpleRenderSystem simpleRenderSystem{ device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		PointLightSystem pointLightSystem{ device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		TexturedRenderSystem texturedRenderSystem{ device, renderer.getSwapChainRenderPass(), texturedSetLayout->getDescriptorSetLayout() };

		// thing
		std::vector<VkDescriptorSet> globalDescriptorSets(FveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			FveDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		std::vector<VkDescriptorSet> texturedDescriptorSets(FveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < texturedDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();

			VkSampler sampler = *fveAssets.createSampler(device, VK_FILTER_LINEAR, "default_sampler");

			Material* texturedMat = fveAssets.getMaterial("texturedmaterial");

			VkDescriptorImageInfo imageBufferInfo;
			imageBufferInfo.sampler = sampler;
			imageBufferInfo.imageView = fveAssets.getTexture("nixon")->imageView;
			imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			FveDescriptorWriter(*texturedSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.writeImage(1, &imageBufferInfo)
				.build(texturedDescriptorSets[i]);

			texturedMat->textureSet = texturedDescriptorSets[i];
		}

		// ================ PREPARE SCENE ================
		loadGameObjects();
		

		FveCamera camera{};
		camera.setViewTarget(glm::vec3(-1, -2, 2), glm::vec3(0.0f, 0.0f, 2.5f));

		auto viewerObject = FveGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
		MovementController cameraController{};
		cameraController.init(window.getGLFWwindow(), fve::WIDTH, fve::HEIGHT);

		auto currentTime = std::chrono::high_resolution_clock::now();

		// persistent uniforms
		GlobalUbo ubo{};

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
					globalDescriptorSets[frameIndex],
					texturedDescriptorSets[frameIndex],
					gameObjects
				};

				// ================ INPUT ================
				// camera controls
				float aspect = renderer.getAspectRatio();
				camera.setPerspectiveProjection(glm::radians(cameraController.fov), aspect, 0.1f, 1000.0f);

				// ================ UPDATE ================

				// update the uniforms
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				pointLightSystem.update(frameInfo, ubo);

				// update the sun position
				auto rotateLight = glm::rotate(glm::mat4(1.0f), frameInfo.frameTime * 0.25f, { 0.0f, 0.0f, -1.0f });
				ubo.sun.lightDirection = rotateLight * ubo.sun.lightDirection;

				// write the uniform changes
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// ================ RENDER ================
				
				// begin offscreen shadow pass
				// render shadow casting objects
				// end offscreen shadow pass

				renderer.beginSwapChainRenderPass(commandBuffer);

				// order matters with transparent objects involved
				simpleRenderSystem.renderGameObjects(frameInfo);
				texturedRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);

				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();

			}

		}

		// wait for the GPU to finish whatever it was doing when the user exits the game
		vkDeviceWaitIdle(device.device());

		std::cout << "Destroying UBO buffers" << std::endl;

	}

	void Game::loadTextures() {

		fveAssets.loadTexture(device, "textures/nixon.png", "nixon");

	}

	void Game::loadGameObjects() {

		// LOAD MESHES
		Mesh* flatVaseMesh = fveAssets.loadMeshFromFile(device, "models/flat_vase.obj", "flat_vase_mesh");
		Mesh* smoothVaseMesh = fveAssets.loadMeshFromFile(device, "models/smooth_vase.obj", "smooth_vase_mesh");
		Mesh* floorMesh = fveAssets.loadMeshFromFile(device, "models/quad.obj", "floor_mesh");

		Material* defaultMaterial = fveAssets.getMaterial("defaultmaterial");
		Material* floorMaterial = fveAssets.getMaterial("texturedmaterial");

		FveModel* flatVaseModel = fveAssets.createModel(device, flatVaseMesh, defaultMaterial, "flat_vase_mat");
		FveModel* smoothVaseModel = fveAssets.createModel(device, smoothVaseMesh, defaultMaterial, "smooth_case_mat");
		FveModel* floorModel = fveAssets.createModel(device, floorMesh, floorMaterial, "floor_mat");
		
		{
			auto flatVase = FveGameObject::createGameObject();
			flatVase.model = flatVaseModel;
			flatVase.transform.translation = { -0.5f, 0.5f, 0.0f };
			flatVase.transform.scale = { 3.0f, 1.5f, 3.0f };
			gameObjects.emplace(flatVase.getId(), std::move(flatVase));
		}

		{
			auto smoothVase = FveGameObject::createGameObject();
			smoothVase.model = smoothVaseModel;
			smoothVase.transform.translation = { 0.5f, 0.5f, 0.0f };
			smoothVase.transform.scale = { 3.0f, 1.5f, 3.0f };
			gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));
		}

		/*{
			auto pointLight = FveGameObject::makePointLight(0.2f);
			gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}*/

		// COLORFUL LIGHTS TIME
		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f}
		};

		for (int i = 0; i < lightColors.size(); i++) {
			auto pointLight = FveGameObject::makePointLight(0.2f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.0f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{0.0f, -1.0f, 0.0f});
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
			gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}

		// CREATE SOMETHING WITH A TEXTURE
		

		//std::cout << "Made a sampler" << std::endl;

		/*if (texturedMat == nullptr) {
			std::cerr << "couldn't find texturedmaterial" << std::endl;
			throw std::runtime_error("lost a material");
		}*/

		//globalPool->allocateDescriptorSet(globalSetLayout->getDescriptorSetLayout(), texturedMat->textureSet);

		/*if (texturedMat->textureSet == VK_NULL_HANDLE) {
			std::cerr << "why didn't that work" << std::endl;
		}

		std::cout << "Made a descriptor set" << std::endl;

		Texture* nixon = fveAssets.getTexture("nixon");
		if (nixon == nullptr) {
			std::cerr << "nixon was impeached" << std::endl;
		}
		else {
			std::cout << "retrieved nixon" << std::endl;
		}

		if (texturedMat->textureSet == VK_NULL_HANDLE) {
			std::cerr << "missing descriptor set" << std::endl;
		}*/

		//FveDescriptorWriter descriptorWriter(*globalSetLayout.get(), *globalPool.get());

		// TEXTURE THE FLOOR
		{
			auto floor = FveGameObject::createGameObject();
			floor.model = floorModel;
			floor.transform.translation = { 0.0f, 0.5f, 0.0f };
			floor.transform.scale = { 3.0f, 1.0f, 3.0f };

			TextureComponent texComp{"nixon"};

			floor.texture = std::make_unique<TextureComponent>(texComp);

			gameObjects.emplace(floor.getId(), std::move(floor));
		}

	}

}