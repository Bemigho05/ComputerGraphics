
#include "memory.h"
#include "single_time_commands.h"

namespace vkUtil {

	uint32_t findMemoryTypeIndex(vk::PhysicalDevice physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties) {
		auto memoryProperties = physicalDevice.getMemoryProperties();

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
			bool supported = static_cast<bool>(supportedMemoryIndices & (1 << i));
			bool sufficient = (memoryProperties.memoryTypes[i].propertyFlags & requestedProperties) == requestedProperties;

			if (supported && sufficient) return i;
		}

		throw("couldn't find as suitable memory type index");
	}

	void allocateBufferMemory(Buffer& buffer, const BufferInput& input) {
		auto memoryRequirements = input.device.getBufferMemoryRequirements(buffer.buffer);
		vk::MemoryAllocateInfo allocInfo;
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryTypeIndex(
			input.physicalDevice, memoryRequirements.memoryTypeBits, input.memoryProperties
		);

		buffer.bufferMemory = input.device.allocateMemory(allocInfo);
		input.device.bindBufferMemory(buffer.buffer, buffer.bufferMemory, 0);
	}

	Buffer createBuffer(BufferInput& input) {
		vk::BufferCreateInfo bufferInfo;
		bufferInfo.flags = vk::BufferCreateFlags();
		bufferInfo.size = input.size;
		bufferInfo.usage = input.usage;
		bufferInfo.sharingMode = vk::SharingMode::eExclusive;


		Buffer buffer;
		buffer.buffer = input.device.createBuffer(bufferInfo);
		allocateBufferMemory(buffer, input);
		return buffer;

	}

	void copyBuffer(Buffer& srcBuffer, Buffer& dstBuffer, vk::DeviceSize size, vk::Queue queue, vk::CommandBuffer commandBuffer)
	{
		start_job(commandBuffer);

		vk::BufferCopy copyRegion;
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;
		commandBuffer.copyBuffer(srcBuffer.buffer, dstBuffer.buffer, 1, &copyRegion);

		end_job(commandBuffer, queue);
	}


}
