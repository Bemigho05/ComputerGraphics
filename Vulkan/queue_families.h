#pragma once
#include "config.h"

namespace vkUtil {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};
	QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface, const bool debug) {
		QueueFamilyIndices indices;

		auto queueFamilies = device.getQueueFamilyProperties();
		if (debug) std::cout << "System can support " << queueFamilies.size() << " queue families.\n";

		uint32_t i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
				indices.graphicsFamily = i;
				if (debug) { std::cout << "Queue family " << i << " is suitable for graphics.\n"; }
			}

			if (device.getSurfaceSupportKHR(i, surface)) {
				indices.presentFamily = i;
				if (debug) { std::cout << "Queue family " << i << " is suitable for presenting.\n"; }

			}
			if (indices.isComplete()) break;
			i++;
		}
		return indices;
	}
}