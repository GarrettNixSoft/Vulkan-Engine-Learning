#pragma once

#include "fve_device.hpp"
#include "fve_buffer.hpp"
#include "fve_types.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace fve {

	class Mesh {
	public:

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadMesh(const std::string& filepath);
		};

		Mesh() = default;

		Mesh(FveDevice& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

		~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		static Mesh createMeshFromFile(FveDevice& device, const std::string& filepath);

		std::unique_ptr<FveBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		bool complexModel = false;
		std::unique_ptr<FveBuffer> indexBuffer;
		uint32_t indexCount;
	private:
		void createVertexBuffers(FveDevice& device, const std::vector<Vertex>& vertices);
		void createIndexBuffers(FveDevice& device, const std::vector<uint32_t>& indices);
	};

	struct Material {
		VkDescriptorSet textureSet{ VK_NULL_HANDLE }; // no texture by default
		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
	};

	class FveModel {
	public:

		FveModel() = default;

		FveModel(FveDevice& device, const std::string& meshId, const std::string& materialId);
		FveModel(FveDevice& device, Mesh* mesh, Material* material);

		~FveModel();

		FveModel(const FveModel&) = delete;
		FveModel& operator=(const FveModel&) = delete;

		virtual inline Mesh& getMesh() const;
		virtual inline Material& getMaterial() const;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		Mesh* mesh;
		Material* material;
	};

}