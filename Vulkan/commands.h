#pragma once
#include "config.h"
#include "queue_families.h"

namespace vkInit {
	struct commandBufferInputChunk {
		vk::Device device;
		vk::CommandPool commandPool;
		std::vector<vkUtil::SwapChainFrame>& frames;
	};

	vk::CommandPool make_command_pool(vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, const bool debug = false) {
		auto queueFamilyIndices = vkUtil::findQueueFamilies(physicalDevice, surface, debug);

		vk::CommandPoolCreateInfo poolInfo = {};
		poolInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		try {
			return device.createCommandPool(poolInfo);
		}
		catch (vk::SystemError err) {
			if (debug) std::cerr << "Failed to create Command Pool" << std::endl;
		}

		return nullptr;
		
	}

	vk::CommandBuffer make_command_buffer(commandBufferInputChunk inputChunk, bool debug) {
		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = inputChunk.commandPool;
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = 1;

		try {
			auto commandBuffer = inputChunk.device.allocateCommandBuffers(allocInfo).at(0);
			if (debug) std::cout << "Allocated main command buffer " << std::endl;

			return commandBuffer;
		}
		catch (vk::SystemError err) {
			if (debug) std::cerr << "Failed to allocate command buffers for frame " << std::endl;
		}

		return nullptr;
	}

	void make_frame_command_buffers(commandBufferInputChunk inputChunk, bool debug) {
		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = inputChunk.commandPool;
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = 1;


		uint32_t i = 0;
		for (auto& frame : inputChunk.frames) {
			try {
				frame.commandBuffer = inputChunk.device.allocateCommandBuffers(allocInfo).at(0);
				if (debug) std::cout << "Allocated command buffer for frame " << i << std::endl;
			}
			catch (vk::SystemError err) {
				if (debug) std::cerr << "Failed to allocated command buffer for frame " << i << std::endl;
			}

			i++;
		}
	}


}