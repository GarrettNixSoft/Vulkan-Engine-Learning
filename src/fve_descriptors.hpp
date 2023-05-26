#pragma once

#include "fve_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace fve {

    class FveDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(FveDevice& device) : device{ device } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<FveDescriptorSetLayout> build() const;

        private:
            FveDevice& device;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        FveDescriptorSetLayout(
            FveDevice& lveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~FveDescriptorSetLayout();
        FveDescriptorSetLayout(const FveDescriptorSetLayout&) = delete;
        FveDescriptorSetLayout& operator=(const FveDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        FveDevice& device;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class FveDescriptorWriter;
    };

    class FveDescriptorPool {
    public:
        class Builder {
        public:
            Builder(FveDevice& device) : device{ device } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<FveDescriptorPool> build() const;

        private:
            FveDevice& device;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        FveDescriptorPool(
            FveDevice& device,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~FveDescriptorPool();
        FveDescriptorPool(const FveDescriptorPool&) = delete;
        FveDescriptorPool& operator=(const FveDescriptorPool&) = delete;

        bool allocateDescriptorSet(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        FveDevice& device;
        VkDescriptorPool descriptorPool;

        friend class FveDescriptorWriter;
    };

    class FveDescriptorWriter {
    public:
        FveDescriptorWriter(FveDescriptorSetLayout& setLayout, FveDescriptorPool& pool);

        FveDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        FveDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        FveDescriptorSetLayout& setLayout;
        FveDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };
}