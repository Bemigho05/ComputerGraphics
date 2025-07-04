#include "swapchain.h"
#include "queue_families.h"
#include "image.h"

#include "logging.h"


vkInit::SwapChainSupportDetails vkInit::query_swapchain_support(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
{
	SwapChainSupportDetails support;

	support.capabilities = device.getSurfaceCapabilitiesKHR(surface);
#ifndef NDEBUG
	std::cout << "Swap chain can support the following surface capabilities:\n"
		<< "\tminimum image count: " << support.capabilities.minImageCount << "\n"
		<< "\tmaximum image count: " << support.capabilities.maxImageCount << "\n"
		<< "\tminimum supported extent: \n"
		<< "\t\twidth: " << support.capabilities.minImageExtent.width << "\n"
		<< "\t\theight: " << support.capabilities.minImageExtent.height << "\n"
		<< "\tmaximum supported extent: \n"
		<< "\t\twidth: " << support.capabilities.maxImageExtent.width << "\n"
		<< "\t\theight: " << support.capabilities.maxImageExtent.height << "\n"
		<< "\tmaximum image array layers: " << support.capabilities.maxImageArrayLayers << "\n";
	std::cout << "\tsupported transform:\n";
	auto stringList = log_transform_bits(support.capabilities.supportedTransforms);
	for (const auto& line : stringList) { std::cout << "\t\t" << line << "\n"; }
	std::cout << "\tcurrent transform:\n";
	stringList = log_transform_bits(support.capabilities.currentTransform);
	for (const auto& line : stringList) { std::cout << "\t\t" << line << "\n"; }
	std::cout << "suppoted alpha operations:\n";
	stringList = log_alpha_composite_bits(support.capabilities.supportedCompositeAlpha);
	for (const auto& line : stringList) { std::cout << "\t\t" << line << "\n"; }
	std::cout << "supported image usage:\n";
	stringList = log_image_usage_bits(support.capabilities.supportedUsageFlags);
	for (const auto& line : stringList) { std::cout << "\t\t" << line << "\n"; }
#endif // !NDEBUG


	support.formats = device.getSurfaceFormatsKHR(surface);
#ifndef NDEBUG
	for (const auto& supportedFormat : support.formats) {
		std::cout << "supported pixel format: " << vk::to_string(supportedFormat.format) << "\n";
		std::cout << "supported color space: " << vk::to_string(supportedFormat.colorSpace) << "\n";
	}
#endif // !NDEBUG

	support.presentModes = device.getSurfacePresentModesKHR(surface);
	for (const auto& presentMode : support.presentModes) {
		std::cout << "\t" << log_present_mode(presentMode) << "\n";
	}

	return support;
}

vk::SurfaceFormatKHR vkInit::choose_swapchain_surface_format(const std::vector<vk::SurfaceFormatKHR>& formats)
{
	for (const auto& format : formats) {
		if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			return format;
		}
	}

	return formats[0];
}

vk::PresentModeKHR vkInit::choose_swapchain_present_mode(const std::vector<vk::PresentModeKHR>& presentModes)
{
	for (const auto& presentMode : presentModes) {
		if (presentMode == vk::PresentModeKHR::eMailbox) {
			return presentMode;
		}
	}

	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D vkInit::choose_swapchain_extent(const uint32_t& width, const uint32_t& height, const vk::SurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		vk::Extent2D extent = { width, height };
		extent.width = std::min(capabilities.maxImageExtent.width, std::max(capabilities.minImageExtent.width, width));
		extent.height = std::min(capabilities.maxImageExtent.height, std::max(capabilities.minImageExtent.height, height));

		return extent;
	}
}

vkInit::SwapChainBundle vkInit::create_swapchain(const vk::Device& logicalDevice, const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const int& width, const int& height)
{
	auto support = query_swapchain_support(physicalDevice, surface);
	auto format = choose_swapchain_surface_format(support.formats);
	auto presentMode = choose_swapchain_present_mode(support.presentModes);
	auto extent = choose_swapchain_extent(width, height, support.capabilities);

	auto imageCount = std::min(support.capabilities.maxImageCount, support.capabilities.minImageCount + 1);


	vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR(
		vk::SwapchainCreateFlagsKHR(), surface, imageCount, format.format, format.colorSpace,
		extent, 1, vk::ImageUsageFlagBits::eColorAttachment
	);


	auto indices = vkUtil::findQueueFamilies(physicalDevice, surface);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else createInfo.imageSharingMode = vk::SharingMode::eExclusive;

	createInfo.preTransform = support.capabilities.currentTransform;
	createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

	SwapChainBundle bundle{};
	try {
		bundle.swapchain = logicalDevice.createSwapchainKHR(createInfo);
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to create swapchain!");
	}

	auto images = logicalDevice.getSwapchainImagesKHR(bundle.swapchain);
	bundle.frames.resize(images.size());

	size_t i = 0;
	for (const auto& image : images) {
		bundle.frames.at(i).image = image;
		bundle.frames.at(i).imageView = vkImage::create_image_view(logicalDevice, image, format.format, vk::ImageAspectFlagBits::eColor);
		i++;

	}

	bundle.format = format.format;
	bundle.extent = extent;

	return bundle;
}