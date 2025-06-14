#pragma once
#define _CRT_SECURE_NO_WARNINGS


#include "config.h"


namespace vkInit{
	bool supported(std::vector<const char*>& extensions, std::vector<const char*>& layers, bool debug) {
		// check extension support

		//check extension support
		auto supportedExtensions = vk::enumerateInstanceExtensionProperties();

		if (debug) {
			std::cout << "Device can support the following extensions:\n";
			for (vk::ExtensionProperties supportedExtension : supportedExtensions) {
				std::cout << '\t' << supportedExtension.extensionName << '\n';
			}
		}

		bool found;
		for (const auto& extension : extensions) {
			found = false;
			for (vk::ExtensionProperties supportedExtension : supportedExtensions) {
				if (strcmp(extension, supportedExtension.extensionName) == 0) {
					found = true;
					if (debug) {
						std::cout << "Extension \"" << extension << "\" is supported!\n";
					}
				}
			}
			if (!found) {
				if (debug) {
					std::cout << "Extension \"" << extension << "\" is not supported!\n";
				}
				return false;
			}
		}

		//check layer support
		std::vector<vk::LayerProperties> supportedLayers = vk::enumerateInstanceLayerProperties();

		if (debug) {
			std::cout << "Device can support the following layers:\n";
			for (const auto& supportedLayer : supportedLayers) {
				std::cout << '\t' << supportedLayer.layerName << '\n';
			}
		}

		for (const auto& layer : layers) {
			found = false;
			for (vk::LayerProperties supportedLayer : supportedLayers) {
				if (strcmp(layer, supportedLayer.layerName) == 0) {
					found = true;
					if (debug) {
						std::cout << "Layer \"" << layer << "\" is supported!\n";
					}
				}
			}
			if (!found) {
				if (debug) {
					std::cout << "Layer \"" << layer << "\" is not supported!\n";
				}
				return false;
			}
		}

		return true;

	}
	vk::Instance make_instance(bool debug, const char* applicationName) {
		auto version = vk::enumerateInstanceVersion();

		if (debug) {
			std::cout << "System can support vulkan Variant: " << VK_API_VERSION_VARIANT(version)
				<< ", Major: " << VK_API_VERSION_MAJOR(version)
				<< ", Minor: " << VK_API_VERSION_MINOR(version)
				<< ", Patch: " << VK_API_VERSION_PATCH(version) << "\n";
		}

		// version &= ~(0xFFFU); // zero out the patch
		// version = VK_MAKE_API_VERSION(0, 1, 0, 0);

		vk::ApplicationInfo appInfo{ applicationName, version, "Doing it the hard way", version, version };

		uint32_t glfwExtensionCount{}, layerCount{};
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		if (debug) { extensions.push_back("VK_EXT_debug_utils"); }

		if (debug) {
			std::cout << "extension to be requested:\n";
			for (const auto& extension : extensions) { std::cout << "\t\"" << extension << "\"\n"; }
		}
		
		//glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		// std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		std::vector<const char*> layers;
		if (debug) { layers.push_back("VK_LAYER_KHRONOS_validation"); }

		if (!supported(extensions, layers, debug)) {
			
			return nullptr;
		};

		auto createInfo = vk::InstanceCreateInfo(
			vk::InstanceCreateFlags(),
			&appInfo,
			static_cast<uint32_t>(layers.size()), layers.data(), // enabled layers
			static_cast<uint32_t>(extensions.size()), extensions.data() // enabled extensions
		);

		try { return vk::createInstance(createInfo); }
		catch (vk::SystemError err) { if (debug) std::cerr << "Failed to create Instance!" << std::endl; return nullptr; }
	}

}