#pragma once

#include <vma/vk_mem_alloc.h>

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
}