#pragma once
#include "config.h"

namespace vkUtil {
	struct BufferInput {
		size_t size;
		vk::BufferUsageFlags usage;
		vk::Device device;
		vk::PhysicalDevice physicalDevice;
		vk::MemoryPropertyFlags memoryProperties;
	};
	struct Buffer {
		vk::Buffer buffer;
		vk::DeviceMemory bufferMemory;
	};

	uint32_t findMemoryTypeIndex(vk::PhysicalDevice physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties);

	void allocateBufferMemory(Buffer& buffer, const BufferInput& input);

	Buffer createBuffer(BufferInput& input);

	void copyBuffer(Buffer& srcBuffer, Buffer& dstBuffer, vk::DeviceSize size, vk::Queue queue, vk::CommandBuffer commandBuffer);

}
