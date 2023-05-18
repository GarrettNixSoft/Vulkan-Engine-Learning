#pragma once

#include "fve_window.h"
#include "fve_pipeline.h"

namespace fve {

	class Game {
	public:
		static constexpr int WIDTH = 1920;
		static constexpr int HEIGHT = 1080;

		void run();
	private:
		FveWindow window{ WIDTH, HEIGHT, "First Vulkan Game" };
		FveDevice device{ window };
		FvePipeline pipeline{device, "shaders/vert.spv", "shaders/frag.spv", FvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
	};

}