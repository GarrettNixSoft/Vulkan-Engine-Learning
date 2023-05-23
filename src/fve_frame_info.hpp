#pragma once

#include "fve_camera.hpp"
#include "fve_game_object.hpp"

#include <vulkan/vulkan.h>

#define MAX_LIGHTS 10

namespace fve {

	struct PointLight {
		glm::vec4 position{}; // ignore w
		glm::vec4 color{}; // w is intensity
	};

	struct GlobalUbo {
		alignas(16) glm::mat4 projection{ 1.0f };
		alignas(16) glm::mat4 view{ 1.0f };
		alignas(16) glm::vec4 ambientLightColor{ 1.0f, 1.0f, 1.0f, 0.02f }; // w is light intensity
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
	};

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		FveCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		FveGameObject::Map& gameObjects;
	};

}