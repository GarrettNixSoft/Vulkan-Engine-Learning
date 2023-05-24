#pragma once

#include <vulkan/vulkan.h>

namespace fve_init {

	VkImageCreateInfo imageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);

}