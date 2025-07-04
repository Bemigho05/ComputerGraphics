#include "queue_families.h"


vkUtil::QueueFamilyIndices vkUtil::findQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
{
	QueueFamilyIndices indices;

	auto queueFamilies = device.getQueueFamilyProperties();
#ifndef NDEBUG
	std::cout << "System can support " << queueFamilies.size() << " queue families.\n";
#endif // !NDEBUG

	uint32_t i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
			indices.graphicsFamily = i;
#ifndef NDEBUG
			std::cout << "Queue family " << i << " is suitable for graphics.\n";
#endif // !NDEBUG
		}

		if (device.getSurfaceSupportKHR(i, surface)) {
			indices.presentFamily = i;
#ifndef NDEBUG
			std::cout << "Queue family " << i << " is suitable for presenting.\n";
#endif // !NDEBUG
		}
		if (indices.isComplete()) break;
		i++;
	}
	return indices;
}