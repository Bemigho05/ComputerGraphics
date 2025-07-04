#pragma once
#include "config.h"

namespace vkInit {
	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	);
	vk::DebugUtilsMessengerEXT make_debug_messenger(vk::Instance& instance, vk::DispatchLoaderDynamic& dldi);

	std::vector<std::string> log_transform_bits(const vk::SurfaceTransformFlagsKHR& bits);

	std::vector<std::string> log_alpha_composite_bits(const vk::CompositeAlphaFlagsKHR& bits);

	std::vector<std::string> log_image_usage_bits(const vk::ImageUsageFlags& bits);

	std::string log_present_mode(const vk::PresentModeKHR& presentMode);

	void log_device_properties(const vk::PhysicalDevice& device);
}