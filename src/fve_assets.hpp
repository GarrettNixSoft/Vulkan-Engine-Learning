#pragma once

#include "fve_model.hpp"

#include <unordered_map>

namespace fve {

	class FveAssets {
	public:
		Material* createMaterial(VkPipeline pipeline, VkPipelineLayout pipelineLayout, const std::string& name);

		Material* getMaterial(const std::string& name);
		Mesh* getMesh(const std::string& name);
	private:
		std::unordered_map<std::string, Material> materials;
		std::unordered_map<std::string, Mesh> meshes;
	};

	extern FveAssets fveAssets;

}