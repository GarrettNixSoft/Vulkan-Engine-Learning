#include "fve_renderer.hpp"
#include "fve_memory.hpp"

#include <iostream>
#include <stdexcept>
#include <array>
#include <cassert>

namespace fve {

	FveRenderer::FveRenderer(FveWindow& window, FveDevice& device) : window { window }, device{ device } {
		recreateSwapChain();
		createCommandBuffers();
	}

	FveRenderer::~FveRenderer() {
		std::cout << "Destroying renderer" << std::endl;
		freeCommandBuffers();
	}

	void FveRenderer::recreateSwapChain() {
		auto extent = window.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = window.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(device.device());
		if (swapChain == nullptr) {
			swapChain = std::make_unique<FveSwapChain>(device, extent);
		}
		else {
			std::shared_ptr<FveSwapChain> oldSwapChain = std::move(swapChain);
			swapChain = std::make_unique<FveSwapChain>(device, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*swapChain.get())) {
				throw std::runtime_error("Swap chain image (or depth) format has changed!");
			}
		}
		
		// TODO
	}

	void FveRenderer::createCommandBuffers() {

		commandBuffers.resize(FveSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = device.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}

	}

	void FveRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(device.device(), device.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer FveRenderer::beginFrame() {
		assert(!isFrameStarted && "Can't call beginFrame() while already in progress");

		auto result = swapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to aquire swap chain image!");
		}

		// this frame is now in progress
		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return commandBuffer;
	}

	void FveRenderer::endFrame() {
		assert(isFrameStarted && "Can't call endFrame() while frame is not in progress");

		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = swapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized()) {
			window.resetWindowResizedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		// this frame has been completed
		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % FveSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void FveRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass() while frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapChain->getRenderPass();
		renderPassInfo.framebuffer = swapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.63f, 0.4f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, swapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void FveRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call endSwapChainRenderPass() while frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}

}