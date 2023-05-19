#pragma once

#pragma once

#include "fve_window.h"
#include "fve_device.h"
#include "fve_swap_chain.h"

#include <cassert>
#include <memory>
#include <vector>

namespace fve {

	class FveRenderer {
	public:

		FveRenderer(FveWindow& window, FveDevice& device);
		~FveRenderer();

		VkRenderPass getSwapChainRenderPass() const {
			return swapChain->getRenderPass();
		}

		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when a frame is not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when a frame is not in progress");
			return currentFrameIndex;

		}

		VkCommandBuffer beginFrame();
		void endFrame();

		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		FveWindow& window;
		FveDevice& device;
		std::unique_ptr<FveSwapChain> swapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool isFrameStarted;

		FveRenderer(const FveRenderer&) = delete;
		FveRenderer& operator=(const FveRenderer&) = delete;

		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();
	};

}