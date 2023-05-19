#pragma once

#include "fve_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace fve {

	class FveModel {
	public:

		struct Vertex {
			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> geAttributeDescriptions();
		};

		FveModel(FveDevice& device, const std::vector<Vertex>& vertices);
		~FveModel();

		FveModel(const FveModel&) = delete;
		FveModel& operator=(const FveModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		FveDevice& device;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;

		void createVertexBuffers(const std::vector<Vertex>& vertices);
	};

}