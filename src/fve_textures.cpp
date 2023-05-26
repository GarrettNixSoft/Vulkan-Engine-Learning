#include "fve_textures.hpp"
#include "fve_assets.hpp"
#include "fve_device.hpp"
#include "fve_initializers.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

#ifdef NDEBUG
const bool debugMode = false;
#else
const bool debugMode = true;
#endif

namespace fve {

	bool loadImageFromFile(FveDevice& device, const char* filePath, AllocatedImage& outImage) {

		int width, height, channels;

		stbi_uc* pixels = stbi_load(filePath, &width, &height, &channels, STBI_rgb_alpha);

		if (!pixels) {
			if (debugMode) std::cerr << "Failed to load texture: " << filePath << std::endl;
			return false;
		}

		void* pixelPtr = pixels;
		VkDeviceSize imageSize = width * height * 4;

		VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB;

		// create a staging buffer
		std::unique_ptr<FveBuffer> stagingBuffer = std::make_unique<FveBuffer>(
			fveAllocator,
			device,
			imageSize,
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_MEMORY_USAGE_CPU_ONLY
		);

		// copy the image data into the staging buffer
		stagingBuffer->map();
		stagingBuffer->writeToBuffer(pixelPtr);

		// image data is now stored in the staging buffer, so we can free it from stbi
		stbi_image_free(pixels);

		// define the image size
		VkExtent3D imageExtent;
		imageExtent.width = static_cast<uint32_t>(width);
		imageExtent.height = static_cast<uint32_t>(height);
		imageExtent.depth = 1;

		// define how the image should be created and used
		VkImageCreateInfo imageInfo = fve_init::imageCreateInfo(imageFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageExtent);

		// prepare to allocate the image
		AllocatedImage newImage;


		// describe how the image should be allocated
		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
		allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		allocInfo.priority = 1.0f;

		// create the image on the GPU
		vmaCreateImage(fveAllocator, &imageInfo, &allocInfo, &newImage.image, &newImage.allocation, nullptr);

		// begin a command buffer to transfer data into the image
		VkCommandBuffer commandBuffer = device.beginSingleTimeCommands();

		// define the image subresources
		VkImageSubresourceRange range;
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		range.baseArrayLayer = 0;
		range.layerCount = 1;

		// define how the image transfer should be done
		VkImageMemoryBarrier imageTransferBarrier{};
		imageTransferBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

		imageTransferBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageTransferBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageTransferBarrier.image = newImage.image;
		imageTransferBarrier.subresourceRange = range;

		imageTransferBarrier.srcAccessMask = 0;
		imageTransferBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		// set the barrier
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageTransferBarrier);

		// define a region to copy
		VkBufferImageCopy copyRegion{};
		copyRegion.bufferOffset = 0;
		copyRegion.bufferRowLength = 0;
		copyRegion.bufferImageHeight = 0;

		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = 0;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount = 1;
		copyRegion.imageExtent = imageExtent; // the whole image

		//copy the buffer into the image
		vkCmdCopyBufferToImage(commandBuffer, stagingBuffer->getAllocatedBuffer().buffer, newImage.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

		// create a barrier for the final format transfer
		VkImageMemoryBarrier imageReadableBarrier = imageTransferBarrier;

		imageReadableBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageReadableBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		imageReadableBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageReadableBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		// set the barrier
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageReadableBarrier);

		// submit the command buffer
		device.endSingleTimeCommands(commandBuffer);

		// confirm load success
		//if (debugMode)
			std::cout << "Loaded texture " << filePath << std::endl;

		// assign the out image and return
		outImage = newImage;
		return true;

	}

}