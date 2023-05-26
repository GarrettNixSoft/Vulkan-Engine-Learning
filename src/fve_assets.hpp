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

		Mesh* loadMeshFromFile(FveDevice& device, const std::string& filepath, const std::string& name);

		Mesh* createMesh(FveDevice& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::string& name);

		Mesh* getMesh(const std::string& name);

		FveModel* createModel(FveDevice& device, Mesh* mesh, Material* material, const std::string& name);

		FveModel* getModel(const std::string& name);

		Texture* getTexture(const std::string& name);

		void loadTexture(FveDevice& device, const std::string& filePath, const std::string& name);

		VkSampler* createSampler(FveDevice& device, VkFilter filters, VkSamplerAddressMode addressMode, const std::string& sampelerId);

		VkSampler* createSampler(FveDevice& device, VkFilter filters, const std::string& sampelerId);

		VkSampler* getSampler(const std::string& samplerId);

		void cleanUp(FveDevice& device);
	private:
		std::unordered_map<std::string, Material> materials;
		std::unordered_map<std::string, Mesh> meshes;

		std::unordered_map<std::string, FveModel> models;

		std::unordered_map<std::string, Texture> textures;

		std::unordered_map<std::string, VkSampler> samplers;
	};

	extern FveAssets fveAssets;

}