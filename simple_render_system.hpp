#pragma once

#pragma once

#include "fve_device.hpp"
#include "fve_game_object.hpp"
#include "fve_pipeline.h"
#include "fve_camera.hpp"
#include "fve_frame_info.hpp"

#include <memory>
#include <vector>

namespace fve {

	class SimpleRenderSystem {
	public:

		SimpleRenderSystem(FveDevice& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		void renderGameObjects(FrameInfo& frameInfo, std::vector<FveGameObject>& gameObjects);

	private:
		FveDevice& device;

		std::unique_ptr<FvePipeline> pipeline;
		VkPipelineLayout pipelineLayout;


		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
	};

}