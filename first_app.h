#pragma once

#include "fve_window.h"
#include "fve_pipeline.h"
#include "fve_device.h"
#include "fve_swap_chain.h"
#include "fve_model.h"

#include <memory>
#include <vector>

namespace fve {

	class Game {
	public:
		static constexpr int WIDTH = 1920;
		static constexpr int HEIGHT = 1080;

		Game();
		~Game();

		void run();
	private:
		FveWindow window{ WIDTH, HEIGHT, "First Vulkan Game" };
		FveDevice device{ window };
		std::unique_ptr<FveSwapChain> swapChain;
		std::unique_ptr<FvePipeline> pipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<FveModel> model;


		Game(const Game&) = delete;
		Game& operator=(const Game&) = delete;

		void loadModels();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
	};

}