#pragma once

#include "fve_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace fve {

	class FveModel {
	public:

		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> geAttributeDescriptions();
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
		};

		FveModel(FveDevice& device, const FveModel::Builder& builder);
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

		bool hasIndexBuffer = false;
		bool complexModel = false;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;

		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);
	};

}