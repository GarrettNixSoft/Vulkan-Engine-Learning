#include "fve_assets.hpp"
#include "fve_utils.hpp"

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

}