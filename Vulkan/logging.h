#pragma once
#include "config.h"

namespace vkInit {
	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
	vk::DebugUtilsMessengerEXT make_debug_messenger(vk::Instance& instance, vk::DispatchLoaderDynamic& dldi) {
		vk::DebugUtilsMessengerCreateInfoEXT createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
			vk::DebugUtilsMessengerCreateFlagsEXT(),
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			debugCallback,
			nullptr
		);

		return instance.createDebugUtilsMessengerEXT(createInfo, nullptr, dldi);
	}

	std::vector<std::string> log_transform_bits(const vk::SurfaceTransformFlagsKHR& bits) {
		std::vector<std::string> result;

		if (bits & vk::SurfaceTransformFlagBitsKHR::eIdentity) { result.push_back("identity"); }
		if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate90) { result.push_back("90 degree rotation"); }
		if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate180) { result.push_back("180 degree rotation"); }
		if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate270) { result.push_back("270 degree rotation"); }
		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirror) { result.push_back("horizontal mirror"); }
		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate90) { result.push_back("horizontal mirror, then 90 degree rotation"); }
		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate180) { result.push_back("horizontal mirror, then 180 degree rotation"); }
		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate270) { result.push_back("horizontal mirror, then 270 degree rotation"); }
		if (bits & vk::SurfaceTransformFlagBitsKHR::eInherit) { result.push_back("inherited"); }

		return result;
	}

	std::vector<std::string> log_alpha_composite_bits(const vk::CompositeAlphaFlagsKHR& bits) {
		std::vector<std::string> result;

		if (bits & vk::CompositeAlphaFlagBitsKHR::eOpaque) { result.push_back("opaque (alpha ignored)"); }
		if (bits & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) { result.push_back("pre multiplied (alpha expected to already be multiplied in image)"); }
		if (bits & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) { result.push_back("post multiplied (alpha will be applied during composition)"); }
		if (bits & vk::CompositeAlphaFlagBitsKHR::eInherit) { result.push_back("inherited"); }

		return result;
	}

	std::vector<std::string> log_image_usage_bits(const vk::ImageUsageFlags& bits) {
		std::vector<std::string> result;
		if (bits & vk::ImageUsageFlagBits::eTransferSrc) {
			result.push_back("transfer src:");
		}
		if (bits & vk::ImageUsageFlagBits::eTransferDst) {
			result.push_back("transfer dst:");
		}
		if (bits & vk::ImageUsageFlagBits::eSampled) {
			result.push_back("sampled: ");
		}
		if (bits & vk::ImageUsageFlagBits::eStorage) {
			result.push_back("storage: ");
		}
		if (bits & vk::ImageUsageFlagBits::eColorAttachment) {
			result.push_back("color attachment: ");
		}
		if (bits & vk::ImageUsageFlagBits::eDepthStencilAttachment) {
			result.push_back("depth/stencil attachment: ");
		}
		if (bits & vk::ImageUsageFlagBits::eTransientAttachment) {
			result.push_back("transient attachment: ");
		}
		if (bits & vk::ImageUsageFlagBits::eInputAttachment) {
			result.push_back("input attachment: ");
		}
		if (bits & vk::ImageUsageFlagBits::eFragmentDensityMapEXT) {
			result.push_back("fragment density map: ");
		}
		if (bits & vk::ImageUsageFlagBits::eFragmentShadingRateAttachmentKHR) {
			result.push_back("fragment shading rate attachment: ");
		}


		return result;
	}

	std::string log_present_mode(const vk::PresentModeKHR& presentMode) {
		if (presentMode == vk::PresentModeKHR::eImmediate) {
			return "immediate: ";
		}
		if (presentMode == vk::PresentModeKHR::eMailbox) {
			return "mailbox: ";
		}
		if (presentMode == vk::PresentModeKHR::eFifo) {
			return "fifo: ";
		}
		if (presentMode == vk::PresentModeKHR::eFifoRelaxed) {
			return "fifo relaxed: ";
		}
		if (presentMode == vk::PresentModeKHR::eSharedContinuousRefresh) {
			return "shared continuous refresh: ";
		}
		if (presentMode == vk::PresentModeKHR::eSharedDemandRefresh) {
			return "shared demand refresh: ";
		}
		return "null";
	}

	void log_device_properties(const vk::PhysicalDevice& device) {
		auto properties = device.getProperties();

		std::cout << "Device name: " << properties.deviceName << "\n";
		std::cout << "Device type: ";
		switch (properties.deviceType) {
		case(vk::PhysicalDeviceType::eCpu):
			std::cout << "CPU\n";
			break;
		case(vk::PhysicalDeviceType::eDiscreteGpu):
			std::cout << "Discrete GPU\n";
			break;
		case(vk::PhysicalDeviceType::eIntegratedGpu):
			std::cout << "Integrated GPU\n";
			break;
		case(vk::PhysicalDeviceType::eVirtualGpu):
			std::cout << "Virtual GPU\n";
			break;
		default: std::cout << "Other\n";
		}
	}
}