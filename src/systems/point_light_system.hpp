#pragma once

#include "fve_device.hpp"
#include "fve_game_object.hpp"
#include "fve_pipeline.hpp"
#include "fve_camera.hpp"
#include "fve_frame_info.hpp"

#include <memory>
#include <vector>

namespace fve {

	class PointLightSystem {
	public:

		PointLightSystem(FveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		void update(FrameInfo& frameInfo, GlobalUbo &ubo);
		void render(FrameInfo& frameInfo);

	private:
		FveDevice& device;

		std::unique_ptr<FvePipeline> pipeline;
		VkPipelineLayout pipelineLayout;


		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);
	};

}