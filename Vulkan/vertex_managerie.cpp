#include "vertex_managerie.h"

VertexManagerie::VertexManagerie()
{
	indexOffset = 0;
}

VertexManagerie::~VertexManagerie()
{
	device.destroyBuffer(vertexBuffer.buffer);
	device.freeMemory(vertexBuffer.bufferMemory);

	device.destroyBuffer(indexBuffer.buffer);
	device.freeMemory(indexBuffer.bufferMemory);
}


void VertexManagerie::consume(meshTypes type, std::vector<float> vertexData, std::vector<uint32_t> indexData)
{
	auto vertexCount = static_cast<int>(vertexData.size() / 8);
	auto indexCount = static_cast<int>(indexData.size());
	auto lastIndex = static_cast<int>(indexLump.size());

	
	firstIndices.insert(std::make_pair(type, lastIndex));
	indexCounts.insert(std::make_pair(type, indexCount));

	for (auto attribute : vertexData)
		vertexLump.push_back(attribute);

	for (auto index : indexData)
		indexLump.push_back(index + indexOffset);

	indexOffset += vertexCount;
}


void VertexManagerie::finalize(FinalizationChunk input)
{
	device = input.device;

	vkUtil::BufferInput inputChunk;
	inputChunk.device = input.device;
	inputChunk.physicalDevice = input.physicalDevice;
	inputChunk.size = sizeof(float) * vertexLump.size();
	inputChunk.usage = vk::BufferUsageFlagBits::eTransferSrc;
	inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	vkUtil::Buffer stagingBuffer = vkUtil::createBuffer(inputChunk);

	auto memoryLocation = input.device.mapMemory(stagingBuffer.bufferMemory, 0, inputChunk.size);
	memcpy(memoryLocation, vertexLump.data(), inputChunk.size);
	input.device.unmapMemory(stagingBuffer.bufferMemory);

	inputChunk.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
	inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
	vertexBuffer = vkUtil::createBuffer(inputChunk);

	vkUtil::copyBuffer(stagingBuffer, vertexBuffer, inputChunk.size, input.queue, input.commandBuffer);


	device.destroyBuffer(stagingBuffer.buffer);
	device.freeMemory(stagingBuffer.bufferMemory);

	inputChunk.size = sizeof(uint32_t) * indexLump.size();
	inputChunk.usage = vk::BufferUsageFlagBits::eTransferSrc;
	inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	stagingBuffer = vkUtil::createBuffer(inputChunk);

	memoryLocation = input.device.mapMemory(stagingBuffer.bufferMemory, 0, inputChunk.size);
	memcpy(memoryLocation, indexLump.data(), inputChunk.size);
	input.device.unmapMemory(stagingBuffer.bufferMemory);

	inputChunk.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
	inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
	indexBuffer = vkUtil::createBuffer(inputChunk);

	vkUtil::copyBuffer(stagingBuffer, indexBuffer, inputChunk.size, input.queue, input.commandBuffer);

	device.destroyBuffer(stagingBuffer.buffer);
	device.freeMemory(stagingBuffer.bufferMemory);

}
