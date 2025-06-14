#pragma once
#include "config.h"
#include "swapchain.h"



namespace vkInit {

	bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& requestedExtensions, const bool& debug) {
		std::set<std::string> requiredExtensions(requestedExtensions.begin(), requestedExtensions.end());
		if (debug) { std::cout << "Device can support extensions:\n"; }
		for (const auto& extension : device.enumerateDeviceExtensionProperties()) {
			if (debug) { std::cout << "\t\"" << extension.extensionName << "\"\n"; }
			requiredExtensions.erase(extension.extensionName);
		}
		return requiredExtensions.empty();
	}

	bool isSuitable(const vk::PhysicalDevice& device, bool debug = false) {
		if (debug) std::cout << "Checking if device is suitable\n";

		const std::vector<const char*> requestedExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		if (debug) {
			std::cout << "We are requesting device extensions:\n";
			for (const auto& extension : requestedExtensions) { std::cout << "\t\"" << extension << "\"\n"; }
		}

		if (const auto& extensionSupported = checkDeviceExtensionSupport(device, requestedExtensions, debug)) {
			if (debug) std::cout << "Device can support the request extensions!\n";

		}
		else {
			if (debug) { std::cout << "Device can't support the requested extensions!\n"; }
			return false;
		}

		return true;
	}

	vk::PhysicalDevice choose_physical_device(vk::Instance& instance, bool debug = false) {
		if (debug) { std::cout << "Choosing physical device...\n"; }
		
		auto availableDevices = instance.enumeratePhysicalDevices();
		
		if (debug) std::cout << "There are " << availableDevices.size() << " physical devices available on this system\n";

		for (const auto& device : availableDevices) {
			if (debug) {
				log_device_properties(device);
			}
			if (isSuitable(device, debug)) { return device; }
		}
		return nullptr;
	}



	vk::Device create_logical_device(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const bool debug = false) {
		auto indices = vkUtil::findQueueFamilies(physicalDevice, surface, debug);
		std::vector<uint32_t> uniqueIndices;
		uniqueIndices.push_back(indices.graphicsFamily.value());
		if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
			uniqueIndices.push_back(indices.presentFamily.value());
		}
		float queuePriority = 1.0f;
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo{};
		for (const auto& queueFamilyIndex : uniqueIndices) {
			queueCreateInfo.push_back(vk::DeviceQueueCreateInfo( vk::DeviceQueueCreateFlags(), queueFamilyIndex, 1, &queuePriority));
		}

		std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		std::vector<const char*> enabledLayers{};
		if (debug) { enabledLayers.push_back("VK_LAYER_KHRONOS_validation"); }

		auto deviceFeatures = vk::PhysicalDeviceFeatures();
		auto deviceInfo = vk::DeviceCreateInfo(
			vk::DeviceCreateFlags(), 
			queueCreateInfo.size(), queueCreateInfo.data(), 
			enabledLayers.size(), enabledLayers.data(), 
			deviceExtensions.size(), deviceExtensions.data(), &deviceFeatures
		);

		try {
			auto device = physicalDevice.createDevice(deviceInfo);
			if (debug) std::cout << "GPU has been successfully abstracted!\n";
			return device;
		}
		catch (vk::SystemError err) { 
			if (debug) std::cout << "GPU hasn't been abstracted!\n";
			return nullptr; 
		}
		
	}

	std::array<vk::Queue, 2> get_queues(const vk::PhysicalDevice& physicalDevice, const vk::Device& device, const vk::SurfaceKHR& surface, const bool debug) {
		auto indices = vkUtil::findQueueFamilies(physicalDevice, surface, debug);

		return
		{ 
			{ device.getQueue(indices.graphicsFamily.value(), 0), device.getQueue(indices.presentFamily.value(), 0) } 
		};
	}

	
}