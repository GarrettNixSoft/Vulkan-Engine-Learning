#pragma once

#include "fve_window.hpp"
#include "fve_types.hpp"

#include <vma/vk_mem_alloc.h>

// std lib headers
#include <string>
#include <vector>

namespace fve {

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct QueueFamilyIndices {
		uint32_t graphicsFamily;
		uint32_t presentFamily;
		bool graphicsFamilyHasValue = false;
		bool presentFamilyHasValue = false;
		bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
	};

	class FveDevice {
	public:
#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif

		FveDevice(FveWindow& window);
		~FveDevice();

		// Not copyable or movable
		FveDevice(const FveDevice&) = delete;
		FveDevice& operator=(const FveDevice&) = delete;
		FveDevice(FveDevice&&) = delete;
		FveDevice& operator=(FveDevice&&) = delete;

		VkInstance instance() { return instance_; }
		VkCommandPool getCommandPool() { return commandPool; }
		VkPhysicalDevice physicalDevice() { return physicalDevice_; }
		VkDevice device() { return device_; }
		VkSurfaceKHR surface() { return surface_; }
		VkQueue graphicsQueue() { return graphicsQueue_; }
		VkQueue presentQueue() { return presentQueue_; }

		SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice_); }
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice_); }
		VkFormat findSupportedFormat(
			const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		// Buffer Helper Functions
		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, VmaAllocation& allocation, VmaMemoryUsage vmaUsage, const char* debugFlag = "defaultDebugFlag");
		AllocatedBuffer allocateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage vmaUsage, const char* debugFlag = "defaultDebugFlag");
		void allocateBuffer(AllocatedBuffer& target, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage vmaUsage, const char* debugFlag = "defaultDebugFlag");

		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void copyBufferToImage(
			VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

		void createImageWithInfo(
			const VkImageCreateInfo& imageInfo,
			VmaAllocationCreateInfo allocCreateInfo,
			VmaAllocation& allocation,
			VmaAllocationInfo& allocInfo,
			VkImage& image);

		VkPhysicalDeviceProperties properties;

	private:
		void createInstance();
		void setupDebugMessenger();
		void createSurface();
		void pickPhysicalDevice();
		void createLogicalDevice();
		void createCommandPool();

		// helper functions
		bool isDeviceSuitable(VkPhysicalDevice device);
		std::vector<const char*> getRequiredExtensions();
		bool checkValidationLayerSupport();
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void hasGflwRequiredInstanceExtensions();
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

		VkDebugUtilsMessengerEXT debugMessenger;
		FveWindow& window;
		VkCommandPool commandPool;

		VkInstance instance_;
		VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
		VkDevice device_;
		VkSurfaceKHR surface_;
		VkQueue graphicsQueue_;
		VkQueue presentQueue_;

		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};

}  // namespace lve