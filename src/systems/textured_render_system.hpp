#pragma once

#include "fve_device.hpp"
#include "fve_game_object.hpp"
#include "fve_pipeline.hpp"
#include "fve_camera.hpp"
#include "fve_frame_info.hpp"

#include <memory>
#include <vector>

namespace fve {

	class TexturedRenderSystem {
	public:

		TexturedRenderSystem(FveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~TexturedRenderSystem();

		void renderGameObjects(FrameInfo& frameInfo);

	private:
		FveDevice& device;

		std::unique_ptr<FvePipeline> pipeline;
		VkPipelineLayout pipelineLayout;


		TexturedRenderSystem(const TexturedRenderSystem&) = delete;
		TexturedRenderSystem& operator=(const TexturedRenderSystem&) = delete;

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);
	};

}