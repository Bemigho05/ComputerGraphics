#pragma once
#include "config.h"

namespace vkInit {
	vk::Semaphore make_semaphore(vk::Device device) {
		vk::SemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.flags = vk::SemaphoreCreateFlags();

		try {
			return device.createSemaphore(semaphoreInfo);
		}
		catch (vk::SystemError err) {
#ifndef NDEBUG
			std::cerr << "Failed to create semaphore" << std::endl;
#endif // !NDEBUG
		}

		return nullptr;
	}

	vk::Fence make_fence(vk::Device device) {
		vk::FenceCreateInfo fenceInfo = {};
		fenceInfo.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

		try {
			return device.createFence(fenceInfo);
		}
		catch (vk::SystemError err) {
#ifndef NDEBUG
			std::cerr << "Failed to create fence" << std::endl;
#endif
		}

		return nullptr;
	}
}