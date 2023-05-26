#include "textured_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>
#include <stdexcept>
#include <array>
#include <cassert>

namespace fve {

	struct SimplePushConstantData {
		alignas(16) glm::mat4 modelMatrix{ 1.0f };
		alignas(16) glm::mat4 normalMatrix{ 1.0f };
	};

	TexturedRenderSystem::TexturedRenderSystem(FveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device{ device } {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	TexturedRenderSystem::~TexturedRenderSystem() {
		vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
	}

	void TexturedRenderSystem::createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout) {
		VkPushConstantRange pushConstantRange;
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ descriptorSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void TexturedRenderSystem::createPipeline(VkRenderPass renderPass) {

		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		FvePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		pipeline = std::make_unique<FvePipeline>(
			device,
			"shaders/textured_shader.vert.spv",
			"shaders/textured_shader.frag.spv",
			pipelineConfig,
			"texturedmaterial");
	}

	void TexturedRenderSystem::renderGameObjects(FrameInfo& frameInfo) {
		pipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&frameInfo.texturedDescriptorSet,
			0,
			nullptr);

		// track mesh/material usage
		Mesh* lastMesh = nullptr;
		Material* lastMaterial = nullptr;

		for (auto& kv : frameInfo.gameObjects) {
			auto& obj = kv.second;

			// skip objects with no model or no texture
			if (obj.model == nullptr) continue;
			if (obj.texture == nullptr) continue;

			//only bind the pipeline if it doesn't match with the already bound one
			if (&obj.model->getMaterial() != lastMaterial) {

				vkCmdBindPipeline(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, obj.model->getMaterial().pipeline);
				lastMaterial = &obj.model->getMaterial();
			}


			SimplePushConstantData push{};
			push.modelMatrix = obj.transform.mat4();
			push.normalMatrix = obj.transform.normalMatrix();

			vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			obj.model->bind(frameInfo.commandBuffer);
			obj.model->draw(frameInfo.commandBuffer);
		}
	}

}