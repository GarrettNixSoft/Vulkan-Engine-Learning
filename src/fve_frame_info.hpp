#pragma once

#include "fve_camera.hpp"
#include "fve_game_object.hpp"

#include <vulkan/vulkan.h>

#define MAX_LIGHTS 10

namespace fve {

	struct Fog {
		glm::vec4 color{ 0.63f, 0.4f, 0.0f, 1.0f };
		glm::vec4 distances{ 50.0f, 70.0f, 0.0f, 0.0f };
	};

	struct Sun {
		glm::vec4 lightDirection{ 0.0f, -10.0f, 0.0f, 0.0f };
		glm::vec4 lightColor{ 1.0f, 1.0f, 1.0f, 0.5f };
	};

	struct PointLight {
		glm::vec4 position{}; // ignore w
		glm::vec4 color{}; // w is intensity
	};

	struct GlobalUbo {
		alignas(16) glm::mat4 projection{ 1.0f };
		alignas(16) glm::mat4 view{ 1.0f };
		alignas(16) glm::mat4 inverseView{ 1.0f };
		alignas(16) glm::vec4 ambientLightColor{ 1.0f, 1.0f, 1.0f, 0.02f }; // w is light intensity
		Fog fog;
		Sun sun;
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