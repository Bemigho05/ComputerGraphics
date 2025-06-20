#include "vertex_managerie.h"

VertexManagerie::VertexManagerie()
{
	offset = 0;
}

VertexManagerie::~VertexManagerie()
{
	logicalDevice.destroyBuffer(vertexBuffer.buffer);
	logicalDevice.freeMemory(vertexBuffer.bufferMemory);
}

void VertexManagerie::consume(meshTypes type, std::vector<float> vertexData)
{
	for (auto attribute : vertexData) {
		lump.push_back(attribute);
	}

	auto vertexCount = static_cast<int>(vertexData.size() / 5);
	offsets.insert(std::make_pair(type, offset));
	sizes.insert(std::make_pair(type, vertexCount));

	offset += vertexCount;
}


void VertexManagerie::finalize(vk::Device device, vk::PhysicalDevice physicalDevice)
{
	this->logicalDevice = logicalDevice;

	vkUtil::BufferInput inputChunk;
	inputChunk.device = device;
	inputChunk.physicalDevice = physicalDevice;
	inputChunk.size = sizeof(float) * lump.size();
	inputChunk.usage = vk::BufferUsageFlagBits::eVertexBuffer;

	vertexBuffer = vkUtil::createBuffer(inputChunk);


	auto memoryLocation = device.mapMemory(vertexBuffer.bufferMemory, 0, inputChunk.size);
	memcpy(memoryLocation, lump.data(), inputChunk.size);
	device.unmapMemory(vertexBuffer.bufferMemory);
}
