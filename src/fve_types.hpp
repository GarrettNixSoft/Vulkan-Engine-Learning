#pragma once

#include <vma/vk_mem_alloc.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace fve {
#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			std::cout <<"Detected Vulkan error: " << err << std::endl; \
			abort();                                                \
		}                                                           \
	} while (0)

	struct AllocatedBuffer {
		VkBuffer buffer;
		VmaAllocation allocation;
	};

	struct AllocatedImage {
		VkImage image;
		VmaAllocation allocation;
	};

	struct Texture {
		AllocatedImage allocatedImage;
		VkImageView imageView;
	};

	struct Vertex {
		glm::vec3 position{};
		glm::vec3 color{};
		glm::vec3 normal{};
		glm::vec2 uv{};

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> geAttributeDescriptions();

		bool operator==(const Vertex& other) const {
			return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
		}
	};
}