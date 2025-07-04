#pragma once
#include "config.h"
#include "memory.h"


struct FinalizationChunk {
	vk::Device device;
	vk::PhysicalDevice physicalDevice;
	vk::Queue queue;
	vk::CommandBuffer commandBuffer;
};

class VertexManagerie {
public:
	VertexManagerie();
	~VertexManagerie();

	void consume(meshTypes type, std::vector<float> vertexData, std::vector<uint32_t> indexData);
	void finalize(FinalizationChunk finalizationChunk);
	vkUtil::Buffer vertexBuffer, indexBuffer;

	std::unordered_map<meshTypes, int> firstIndices;
	std::unordered_map<meshTypes, int> indexCounts;
private:
	int indexOffset;
	vk::Device device;
	std::vector<float> vertexLump;
	std::vector<uint32_t> indexLump;
};