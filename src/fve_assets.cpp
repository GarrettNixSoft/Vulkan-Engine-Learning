#include "fve_assets.hpp"
#include "fve_utils.hpp"
#include "fve_initializers.hpp"
#include "fve_buffer.hpp"

#include <stdexcept>
#include <iostream>

#ifndef ENGINE_DIR
#define ENGINE_DIR "../"
#endif

class FveBuffer;

namespace std {

	template<>
	struct hash<fve::Material> {
		size_t operator()(fve::Material const& material) const {
			size_t seed = 0;
			fve::hashCombine(seed, material.pipeline, material.pipelineLayout);
			return seed;
		}
	};

	template<>
	struct hash<fve::Mesh> {
		size_t operator()(fve::Mesh const& mesh) const {
			size_t seed = 0;
			fve::hashCombine(seed, mesh.vertexBuffer, mesh.indexBuffer);
			return seed;
		}
	};

	template<>
	struct hash<fve::FveModel> {
		size_t operator()(fve::FveModel const& model) const {
			size_t seed = 0;
			fve::hashCombine(seed, model.getMesh(), model.getMaterial());
			return seed;
		}
	};

}

namespace fve {

	FveAssets fveAssets;

	FveAssets::~FveAssets() {
		//
	}

	Material* FveAssets::createMaterial(VkPipeline pipeline, VkPipelineLayout pipelineLayout, const std::string& matId) {
		Material mat;
		mat.pipeline = pipeline;
		mat.pipelineLayout = pipelineLayout;
		materials.emplace(matId, mat);
		return &materials[matId];
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

	Mesh* FveAssets::loadMeshFromFile(FveDevice& device, const std::string& filepath, const std::string& meshId) {

		// check if the mesh already exists
		Mesh* existing = getMesh(meshId);
		if (existing != nullptr) {
			std::cerr << "Tried to load a mesh that already exists! (id: " << meshId << ")" << std::endl;
			return existing;
		}

		Mesh::Builder builder;
		builder.loadMesh(filepath);

		return createMesh(device, builder.vertices, builder.indices, meshId);

	}

	Mesh* FveAssets::createMesh(FveDevice& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::string& meshId) {
		
		// check if the mesh already exists
		Mesh* existing = getMesh(meshId);
		if (existing != nullptr) {
			std::cerr << "Tried to create a mesh that already exists! (id: " << meshId << ")" << std::endl;
			return existing;
		}

		//Mesh mesh = Mesh(device, vertices, indices);
		meshes.try_emplace(meshId, device, vertices, indices);
		return &meshes[meshId];

	}

	Mesh* FveAssets::getMesh(const std::string& meshId) {

		auto it = meshes.find(meshId);
		if (it == meshes.end()) {
			return nullptr;
		}
		else {
			return &(*it).second;
		}

	}

	FveModel* FveAssets::createModel(FveDevice& device, Mesh* mesh, Material* material, const std::string& modelId) {

		// check if the model already exists
		FveModel* existing = getModel(modelId);
		if (existing != nullptr) {
			std::cerr << "Tried to create a model that already exists! (id: " << modelId << ")" << std::endl;
			return existing;
		}

		//FveModel model = FveModel(device, mesh, material);
		models.try_emplace(modelId, device, mesh, material);
		return &models[modelId];

	}

	FveModel* FveAssets::getModel(const std::string& modelId) {

		auto it = models.find(modelId);
		if (it == models.end()) {
			return nullptr;
		}
		else {
			return &(*it).second;
		}

	}

	Texture* FveAssets::getTexture(const std::string& textureId) {

		auto it = textures.find(textureId);
		if (it == textures.end()) {
			return nullptr;
		}
		else {
			return &(*it).second;
		}

	}

	void FveAssets::loadTexture(FveDevice& device, const std::string& filePath, const std::string& textureId) {

		// TODO check already exists

		std::string enginePath = ENGINE_DIR + filePath;

		Texture texture;
		if (loadImageFromFile(device, enginePath.c_str(), texture.allocatedImage)) {

			VkImageViewCreateInfo imageinfo = fve_init::imageViewCreateInfo(VK_FORMAT_R8G8B8A8_SRGB, texture.allocatedImage.image, VK_IMAGE_ASPECT_COLOR_BIT);
			vkCreateImageView(device.device(), &imageinfo, nullptr, &texture.imageView);

			textures.emplace(textureId, texture);
		}
		else throw std::runtime_error("Failed to load texture " + filePath);

	}

	void FveAssets::cleanUp() {
		for (auto& kv : meshes) {
			auto& mesh = kv.second;
			vmaDestroyBuffer(fveAllocator, mesh.vertexBuffer->getAllocatedBuffer().buffer, mesh.vertexBuffer->getAllocatedBuffer().allocation);
		}
		for (auto& kv : textures) {
			auto& texture = kv.second;
			vmaDestroyImage(fveAllocator, texture.allocatedImage.image, texture.allocatedImage.allocation);
			std::cout << "Cleaned up " << kv.first << std::endl;
		}
	}

}