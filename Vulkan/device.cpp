#include "device.h"
#include "queue_families.h"
#include "logging.h"


bool vkInit::checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& requestedExtensions)
{
	std::set<std::string> requiredExtensions(requestedExtensions.begin(), requestedExtensions.end());
#ifndef NDEBUG
	std::cout << "Device can support extensions:\n";
#endif // !NDEBUG
	for (const auto& extension : device.enumerateDeviceExtensionProperties()) {
#ifndef NDEBUG
		std::cout << "\t\"" << extension.extensionName << "\"\n";
#endif // !NDEBUG
		requiredExtensions.erase(extension.extensionName);
	}
	return requiredExtensions.empty();
}

bool vkInit::isSuitable(const vk::PhysicalDevice& device)
{
#ifndef NDEBUG
	std::cout << "Checking if device is suitable\n";
#endif // !NDEBUG

	const std::vector<const char*> requestedExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
#ifndef NDEBUG
	std::cout << "We are requesting device extensions:\n";
	for (const auto& extension : requestedExtensions) { std::cout << "\t\"" << extension << "\"\n"; }
#endif // !NDEBUG

	if (const auto& extensionSupported = checkDeviceExtensionSupport(device, requestedExtensions)) {
#ifndef NDEBUG
		std::cout << "Device can support the request extensions!\n";
#endif // !NDEBUG
	}
	else {
#ifndef NDEBUG
		std::cout << "Device can't support the requested extensions!\n";
#endif // !NDEBUG
		return false;
	}

	return true;
}

vk::PhysicalDevice vkInit::choose_physical_device(vk::Instance& instance)
{
#ifndef NDEBUG
	std::cout << "Choosing physical device...\n";
#endif // !NDEBUG

	auto availableDevices = instance.enumeratePhysicalDevices();
#ifndef NDEBUG
	std::cout << "There are " << availableDevices.size() << " physical devices available on this system\n";
#endif // !NDEBUG

	for (const auto& device : availableDevices) {

#ifndef NDEBUG
		log_device_properties(device);
#endif // NDEBUG

		if (isSuitable(device)) { return device; }
	}
	return nullptr;
}

vk::Device vkInit::create_logical_device(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface)
{
	auto indices = vkUtil::findQueueFamilies(physicalDevice, surface);
	std::vector<uint32_t> uniqueIndices;
	uniqueIndices.push_back(indices.graphicsFamily.value());
	if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
		uniqueIndices.push_back(indices.presentFamily.value());
	}
	float queuePriority = 1.0f;
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo{};
	for (const auto& queueFamilyIndex : uniqueIndices) {
		queueCreateInfo.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), queueFamilyIndex, 1, &queuePriority));
	}

	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	std::vector<const char*> enabledLayers{};
#ifndef NDEBUG
	enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif // !NDEBUG


	auto deviceFeatures = vk::PhysicalDeviceFeatures();
	auto deviceInfo = vk::DeviceCreateInfo(
		vk::DeviceCreateFlags(),
		queueCreateInfo.size(), queueCreateInfo.data(),
		enabledLayers.size(), enabledLayers.data(),
		deviceExtensions.size(), deviceExtensions.data(), &deviceFeatures
	);

	try {
		auto device = physicalDevice.createDevice(deviceInfo);
#ifndef NDEBUG
		std::cout << "GPU has been successfully abstracted!\n";
#endif // !NDEBUG
		return device;
	}
	catch (vk::SystemError err) {
#ifndef NDEBUG
		std::cout << "GPU hasn't been abstracted!\n";
#endif // !NDEBUG
		return nullptr;
	}

}

std::array<vk::Queue, 2> vkInit::get_queues(const vk::PhysicalDevice& physicalDevice, const vk::Device& device, const vk::SurfaceKHR& surface)
{
	auto indices = vkUtil::findQueueFamilies(physicalDevice, surface);
	return
	{
		{ device.getQueue(indices.graphicsFamily.value(), 0), device.getQueue(indices.presentFamily.value(), 0) }
	};
}
