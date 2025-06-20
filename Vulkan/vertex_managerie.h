#pragma once
#include "config.h"
#include "memory.h"


class VertexManagerie {
public:
	VertexManagerie();
	~VertexManagerie();

	void consume(meshTypes type, std::vector<float> vertexData);
	void finalize(vk::Device device, vk::PhysicalDevice physicalDevice);
	vkUtil::Buffer vertexBuffer;

	std::unordered_map<meshTypes, int> offsets;
	std::unordered_map<meshTypes, int> sizes;
private:
	int offset;
	vk::Device logicalDevice;
	std::vector<float> lump;
};