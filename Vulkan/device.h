#pragma once
#include "config.h"

namespace vkInit {

	bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& requestedExtensions);

	bool isSuitable(const vk::PhysicalDevice& device);

	vk::PhysicalDevice choose_physical_device(vk::Instance& instance);

	vk::Device create_logical_device(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface);

	std::array<vk::Queue, 2> get_queues(const vk::PhysicalDevice& physicalDevice, const vk::Device& device, const vk::SurfaceKHR& surface);
	
}