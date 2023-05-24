#include "fve_assets.hpp"
#include "fve_utils.hpp"

#include <stdexcept>
#include <iostream>

#ifndef ENGINE_DIR
#define ENGINE_DIR "../"
#endif

namespace std {

	template<>
	struct hash<fve::Material> {
		size_t operator()(fve::Material const& material) const {
			size_t seed = 0;
			fve::hashCombine(seed, material.pipeline, material.pipelineLayout);
			return seed;
		}
	};

}

namespace fve {

	FveAssets fveAssets;

	FveAssets::~FveAssets() {
		//
	}

	Material* FveAssets::createMaterial(VkPipeline pipeline, VkPipelineLayout pipelineLayout, const std::string& name) {
		Material mat;
		mat.pipeline = pipeline;
		mat.pipelineLayout = pipelineLayout;
		materials.emplace(name, mat);
		return &materials[name];
	}

	Material* FveAssets::getMaterial(const std::string& name) {

		auto it = materials.find(name);
		if (it == materials.end()) {
			return nullptr;
		}
		else {
			return &(*it).second;
		}

	}

	Mesh* FveAssets::getMesh(const std::string& name) {

		auto it = meshes.find(name);
		if (it == meshes.end()) {
			return nullptr;
		}
		else {
			return &(*it).second;
		}

	}

	void FveAssets::loadTexture(FveDevice& device, const std::string& filePath, const std::string& name) {

		std::string enginePath = ENGINE_DIR + filePath;

		AllocatedImage texture;
		if (loadImageFromFile(device, enginePath.c_str(), texture)) {
			textures.emplace(name, texture);
		}
		else throw std::runtime_error("Failed to load texture " + filePath);

	}

	void FveAssets::cleanUp() {
		for (auto& kv : meshes) {
			auto& mesh = kv.second;
			vmaDestroyBuffer(fveAllocator, mesh.vertexBuffer.buffer, mesh.vertexBuffer.allocation);
		}
		for (auto& kv : textures) {
			auto& texture = kv.second;
			vmaDestroyImage(fveAllocator, texture.image, texture.allocation);
			std::cout << "Cleaned up " << kv.first << std::endl;
		}
	}

}