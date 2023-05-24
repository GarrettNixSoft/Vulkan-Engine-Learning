#pragma once

#include "fve_model.hpp"
#include "fve_memory.hpp"
#include "fve_device.hpp"
#include "fve_textures.hpp"

#include <unordered_map>

namespace fve {

	class FveAssets {
	public:

		FveAssets() = default;
		~FveAssets();

		FveAssets(const FveAssets&) = delete;
		FveAssets& operator=(const FveAssets&) = delete;
		FveAssets(FveAssets&&) = delete;
		FveAssets& operator=(FveAssets&&) = delete;


		Material* createMaterial(VkPipeline pipeline, VkPipelineLayout pipelineLayout, const std::string& name);

		Material* getMaterial(const std::string& name);
		Mesh* getMesh(const std::string& name);

		void loadTexture(FveDevice& device, const std::string& filePath, const std::string& name);

		void cleanUp();
	private:
		std::unordered_map<std::string, Material> materials;
		std::unordered_map<std::string, Mesh> meshes;

		std::unordered_map<std::string, AllocatedImage> textures;
	};

	extern FveAssets fveAssets;

}