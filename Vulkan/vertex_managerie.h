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
	~VertexManagerie() = default;

	void consume(meshTypes type, std::vector<float> vertexData);
	void finalize(FinalizationChunk finalizationChunk);
	vkUtil::Buffer vertexBuffer;

	std::unordered_map<meshTypes, int> offsets;
	std::unordered_map<meshTypes, int> sizes;
private:
	int offset;
	vk::Device logicalDevice;
	std::vector<float> lump;
};