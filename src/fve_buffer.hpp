#pragma once

#include "fve_device.hpp"
#include "fve_types.hpp"

#include <vma/vk_mem_alloc.h>

namespace fve {

    class FveBuffer {
    public:
        FveBuffer(
            VmaAllocator allocator,
            FveDevice& device,
            VkDeviceSize instanceSize,
            uint32_t instanceCount,
            VkBufferUsageFlags usageFlags,
            VmaMemoryUsage vmaUsage,
            VkDeviceSize minOffsetAlignment = 1);
        ~FveBuffer();

        FveBuffer(const FveBuffer&) = delete;
        FveBuffer& operator=(const FveBuffer&) = delete;

        VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void unmap();

        void writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        void writeToIndex(void* data, int index);
        VkResult flushIndex(int index);
        VkDescriptorBufferInfo descriptorInfoForIndex(int index);
        VkResult invalidateIndex(int index);

        AllocatedBuffer getAllocatedBuffer() const { return buffer; }
        void* getMappedMemory() const { return mapped; }
        uint32_t getInstanceCount() const { return instanceCount; }
        VkDeviceSize getInstanceSize() const { return instanceSize; }
        VkDeviceSize getAlignmentSize() const { return instanceSize; }
        VkBufferUsageFlags getUsageFlags() const { return usageFlags; }
        VkDeviceSize getBufferSize() const { return bufferSize; }

    private:
        static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

        FveDevice& device;
        VmaAllocator allocator;

        void* mapped = nullptr;
        AllocatedBuffer buffer;

        VkDeviceSize bufferSize;
        uint32_t instanceCount;
        VkDeviceSize instanceSize;
        VkDeviceSize alignmentSize;
        VkBufferUsageFlags usageFlags;
    };

}
