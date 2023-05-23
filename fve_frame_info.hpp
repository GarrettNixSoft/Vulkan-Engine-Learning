#pragma once

#include "fve_camera.hpp"
#include "fve_game_object.hpp"

#include <vulkan/vulkan.h>

namespace fve {

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		FveCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		FveGameObject::Map& gameObjects;
	};;

}