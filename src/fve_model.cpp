#include "fve_model.hpp"
#include "fve_utils.hpp"
#include "fve_memory.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <unordered_map>
#include <iostream>
#include <cassert>
#include <limits>

#ifndef ENGINE_DIR
#define ENGINE_DIR "../"
#endif

namespace std {

	template<>
	struct hash<fve::Vertex> {
		size_t operator()(fve::Vertex const& vertex) const {
			size_t seed = 0;
			fve::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};

}

namespace fve {

	FveModel::FveModel(FveDevice& device, const FveModel::Builder& builder) : device{ device } {
		createVertexBuffers(builder.mesh.vertices);
		createIndexBuffers(builder.indices);
	}

	FveModel::~FveModel() {}

	std::unique_ptr<FveModel> FveModel::createModelFromFile(FveDevice& device, const std::string& filepath) {
		Builder builder;
		builder.loadModel(filepath);
		std::cout << "Loaded model: " << filepath << " -- " << "Vertex count: " << builder.mesh.vertices.size() << std::endl;
		return std::make_unique<FveModel>(device, builder);
	}

	void FveModel::createVertexBuffers(const std::vector<Vertex>& vertices) {
		// count the vertices, veryfi we have at least 3
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");

		// compute the size of the buffer we need
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

		// TODO: automatically use uint16_t for simple models
		complexModel = vertexCount > std::numeric_limits<uint16_t>::max();
		
		// create a staging buffer
		uint32_t vertexSize = sizeof(vertices[0]);

		FveBuffer stagingBuffer{
			fveAllocator,
			device,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_MEMORY_USAGE_CPU_TO_GPU
		};

		// copy the vertex data into the staging buffer
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());

		// create a device local buffer on the GPU
		vertexBuffer = std::make_unique<FveBuffer>(
			fveAllocator,
			device,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VMA_MEMORY_USAGE_GPU_ONLY
		);

		// copy the staging buffer contents into the device local buffer
		device.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
	}

	void FveModel::createIndexBuffers(const std::vector<uint32_t>& indices) {
		// count the indices, determine if we're using an index buffer for this model
		indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = indexCount > 0;

		// if we have no indices, this model is not using an index buffer
		if (!hasIndexBuffer) return;

		// compute the size of the buffer we need
		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

		uint32_t indexSize = sizeof(indices[0]);

		// create a staging buffer
		FveBuffer stagingBuffer{
			fveAllocator,
			device,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_MEMORY_USAGE_CPU_TO_GPU
		};

		// copy the index data into the staging buffer
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());

		// create a device local buffer on the GPU
		indexBuffer = std::make_unique<FveBuffer>(
			fveAllocator,
			device,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VMA_MEMORY_USAGE_GPU_ONLY
		);

		// copy the staging buffer contents into the device local buffer
		device.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
	}

	void FveModel::draw(VkCommandBuffer commandBuffer) {
		if (hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
		}
	}

	void FveModel::bind(VkCommandBuffer commandBuffer) {
		VkBuffer buffers[] = { vertexBuffer->getBuffer()};
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		if (hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	std::vector<VkVertexInputBindingDescription> Vertex::getBindingDescriptions() {
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Vertex::geAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
		attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
		attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
		attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});


		return attributeDescriptions;
	}

	void FveModel::Builder::loadModel(const std::string& filepath) {

		std::string enginePath = ENGINE_DIR + filepath;

		// prepare what tinyobjloader needs to load an OBJ file
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;
		// load the OBJ file with tinyobjloader
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, enginePath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		mesh.vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				// index values are optional
				if (index.vertex_index >= 0) {
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};
					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2]
					};
				}

				// normals are optional
				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}

				// tex coords are optional
				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}

				// store the vertex
				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(mesh.vertices.size());
					mesh.vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}

		// ...
	}

}