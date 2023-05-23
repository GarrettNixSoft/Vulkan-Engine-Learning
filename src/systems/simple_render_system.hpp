#pragma once

#include "fve_device.hpp"
#include "fve_game_object.hpp"
#include "fve_pipeline.hpp"
#include "fve_camera.hpp"
#include "fve_frame_info.hpp"

#include <memory>
#include <vector>

namespace fve {

	class SimpleRenderSystem {
	public:

		SimpleRenderSystem(FveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		void renderGameObjects(FrameInfo& frameInfo);

	private:
		FveDevice& device;

		std::unique_ptr<FvePipeline> pipeline;
		VkPipelineLayout pipelineLayout;


		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);
	};

}