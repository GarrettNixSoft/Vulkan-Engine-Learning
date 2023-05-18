#pragma once

#include "fve_device.h"

#include <string>
#include <vector>

namespace fve {

	struct PipelineConfigInfo {};

	class FvePipeline {
	public:
		FvePipeline(FveDevice& device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);

		~FvePipeline() {}

		FvePipeline(const FvePipeline&) = delete;
		void operator=(const FvePipeline&) = delete;

		static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
		
	private:
		static std::vector<char> readFile(const std::string& filepath);
		FveDevice& fveDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;

		void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
	};


}