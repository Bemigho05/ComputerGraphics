#include "vertex_managerie.h"

VertexManagerie::VertexManagerie()
{
	offset = 0;
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


void VertexManagerie::finalize(FinalizationChunk finalizeInput)
{
	logicalDevice = finalizeInput.device;

	vkUtil::BufferInput inputChunk;
	inputChunk.device = finalizeInput.device;
	inputChunk.physicalDevice = finalizeInput.physicalDevice;
	inputChunk.size = sizeof(float) * lump.size();
	inputChunk.usage = vk::BufferUsageFlagBits::eTransferSrc;
	inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	vkUtil::Buffer stagingBuffer = vkUtil::createBuffer(inputChunk);

	auto memoryLocation = finalizeInput.device.mapMemory(stagingBuffer.bufferMemory, 0, inputChunk.size);
	memcpy(memoryLocation, lump.data(), inputChunk.size);
	finalizeInput.device.unmapMemory(stagingBuffer.bufferMemory);

	inputChunk.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
	inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
	vertexBuffer = vkUtil::createBuffer(inputChunk);

	vkUtil::copyBuffer(stagingBuffer, vertexBuffer, inputChunk.size, finalizeInput.queue, finalizeInput.commandBuffer);


	logicalDevice.destroyBuffer(stagingBuffer.buffer);
	logicalDevice.freeMemory(stagingBuffer.bufferMemory);

}
