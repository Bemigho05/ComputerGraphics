#include "image.h"
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include "memory.h"
#include "descriptor.h"
#include "single_time_commands.h"

vkImage::Texture::Texture(TextureInput input)
	: device(input.device), physicalDevice(input.physicalDevice), filename(input.filename),
	commandBuffer(input.commandBuffer), queue(input.queue), layout(input.layout), descriptorPool(input.descriptorPool)
{
	load();

	ImageCreateInput imageInput;
	imageInput.device = device;
	imageInput.physicalDevice = physicalDevice;
	imageInput.height = height;
	imageInput.width = width;
	imageInput.tiling = vk::ImageTiling::eOptimal;
	imageInput.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
	imageInput.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
	imageInput.format = vk::Format::eR8G8B8A8Unorm;

	image = create_image(imageInput);
	imageMemory = create_image_memory(imageInput, image);

	populate();

	free(pixels);

	create_view();

	create_sampler();

	create_descriptor_set();

}

void vkImage::Texture::use(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout)
{
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 1, descriptorSet, nullptr);

}

vkImage::Texture::~Texture()
{
	device.freeMemory(imageMemory);
	device.destroyImage(image);
	device.destroyImageView(imageView);
	device.destroySampler(sampler);
	
}

void vkImage::Texture::load()
{
	pixels = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	if (!pixels) {
#ifndef NDEBUG
		std::cerr << "Failed to load: " << filename << std::endl;
#endif
	}
}

void vkImage::Texture::populate()
{
	vkUtil::BufferInput input;
	input.device = device;
	input.physicalDevice = physicalDevice;
	input.memoryProperties = vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible;
	input.usage = vk::BufferUsageFlagBits::eTransferSrc;
	input.size = width * height * 4;

	auto stagingBuffer = vkUtil::createBuffer(input);

	auto writeLocation = device.mapMemory(stagingBuffer.bufferMemory, 0, input.size);
	memcpy(writeLocation, pixels, input.size);
	device.unmapMemory(stagingBuffer.bufferMemory);

	ImageLayoutTransitionInput transitionInput;
	transitionInput.commandBuffer = commandBuffer;
	transitionInput.queue = queue;
	transitionInput.image = image;
	transitionInput.oldlayout = vk::ImageLayout::eUndefined;
	transitionInput.newlayout = vk::ImageLayout::eTransferDstOptimal;
	transition_image_layout(transitionInput);

	BufferImageCopyInput copyInput;
	copyInput.commandBuffer = commandBuffer;
	copyInput.queue = queue;
	copyInput.srcBuffer = stagingBuffer.buffer;
	copyInput.dstImage = image;
	copyInput.width = width;
	copyInput.height = height;
	copy_buffer_to_image(copyInput);

	transitionInput.oldlayout = vk::ImageLayout::eTransferDstOptimal;
	transitionInput.newlayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	transition_image_layout(transitionInput);


	device.freeMemory(stagingBuffer.bufferMemory);
	device.destroyBuffer(stagingBuffer.buffer);
	
}

void vkImage::Texture::create_view()
{
	imageView = create_image_view(device, image, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);
}

void vkImage::Texture::create_sampler()
{
	vk::SamplerCreateInfo samplerInfo;
	samplerInfo.flags = vk::SamplerCreateFlags();
	samplerInfo.minFilter = vk::Filter::eNearest;
	samplerInfo.magFilter = vk::Filter::eLinear;
	samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
	samplerInfo.anisotropyEnable = false; // TODO: 
	samplerInfo.maxAnisotropy = 1.0f;
	samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
	samplerInfo.unnormalizedCoordinates = false;
	samplerInfo.compareEnable = false;
	samplerInfo.compareOp = vk::CompareOp::eAlways;
	samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	try {
		sampler = device.createSampler(samplerInfo);
	}
	catch (vk::SystemError err) {
#ifndef NDEBUG
		std::cerr << "Failed to create sampler" << std::endl;
#endif
	}
}

void vkImage::Texture::create_descriptor_set()
{
	descriptorSet = vkInit::allocate_descriptor_set(device, descriptorPool, layout);
	vk::DescriptorImageInfo imageDescriptor;
	imageDescriptor.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	imageDescriptor.imageView = imageView;
	imageDescriptor.sampler = sampler;

	vk::WriteDescriptorSet descriptorWrite;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &imageDescriptor;

	device.updateDescriptorSets(descriptorWrite, nullptr);

}

vk::Image vkImage::create_image(ImageCreateInput input)
{
	vk::ImageCreateInfo imageInfo;
	imageInfo.flags = vk::ImageCreateFlagBits();
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.extent = vk::Extent3D(input.width, input.height, 1);
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = input.format;
	imageInfo.tiling = input.tiling;
	imageInfo.initialLayout = vk::ImageLayout::eUndefined;
	imageInfo.usage = input.usage;
	imageInfo.sharingMode = vk::SharingMode::eExclusive;
	imageInfo.samples = vk::SampleCountFlagBits::e1;

	try {
		return input.device.createImage(imageInfo);
	}
	catch (vk::SystemError err) {
#ifndef NDEBUG
		std::cout << "Unable to create Image" << std::endl;
#endif
	}
}

vk::DeviceMemory vkImage::create_image_memory(ImageCreateInput input, vk::Image image)
{
	auto requirements = input.device.getImageMemoryRequirements(image);

	vk::MemoryAllocateInfo allocation;
	allocation.allocationSize = requirements.size;
	allocation.memoryTypeIndex = vkUtil::findMemoryTypeIndex(input.physicalDevice, requirements.memoryTypeBits, input.memoryProperties);

	try {
		vk::DeviceMemory imageMemory = input.device.allocateMemory(allocation);
		input.device.bindImageMemory(image, imageMemory, 0);
		return imageMemory;
	}
	catch (vk::SystemError err) {
#ifndef NDEBUG
		std::cerr << "Unable to allocate memory for image" << std::endl;
#endif
	}

	
}

void vkImage::transition_image_layout(ImageLayoutTransitionInput input)
{
	vkUtil::start_job(input.commandBuffer);
	vk::ImageSubresourceRange access;
	access.aspectMask = vk::ImageAspectFlagBits::eColor;
	access.baseMipLevel = 0;
	access.levelCount = 1;
	access.baseArrayLayer = 0;
	access.layerCount = 1;

	vk::ImageMemoryBarrier barrier;
	barrier.oldLayout = input.oldlayout;
	barrier.newLayout = input.newlayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = input.image;
	barrier.subresourceRange = access;

	vk::PipelineStageFlags srcStage, dstStage;

	if (input.oldlayout == vk::ImageLayout::eUndefined
		&& input.newlayout == vk::ImageLayout::eTransferDstOptimal) {

		barrier.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
		dstStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else {

		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		srcStage = vk::PipelineStageFlagBits::eTransfer;
		dstStage = vk::PipelineStageFlagBits::eFragmentShader;
	}

	input.commandBuffer.pipelineBarrier(srcStage, dstStage, vk::DependencyFlags(), nullptr, nullptr, barrier);

	vkUtil::end_job(input.commandBuffer, input.queue);
}

void vkImage::copy_buffer_to_image(BufferImageCopyInput input)
{
	vkUtil::start_job(input.commandBuffer);

	vk::BufferImageCopy copy;
	copy.bufferOffset = 0;
	copy.bufferRowLength = 0;
	copy.bufferImageHeight = 0;

	vk::ImageSubresourceLayers access;
	access.aspectMask = vk::ImageAspectFlagBits::eColor;
	access.mipLevel = 0;
	access.baseArrayLayer = 0;
	access.layerCount = 1;
	copy.imageSubresource = access;

	copy.imageOffset = vk::Offset3D(0, 0, 0);
	copy.imageExtent = vk::Extent3D(input.width, input.height, 1.0f);

	input.commandBuffer.copyBufferToImage(input.srcBuffer, input.dstImage, vk::ImageLayout::eTransferDstOptimal, copy);

	vkUtil::end_job(input.commandBuffer, input.queue);

}

vk::ImageView vkImage::create_image_view(vk::Device device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspect)
{
	vk::ImageViewCreateInfo createInfo{};
	createInfo.image = image;
	createInfo.format = format;
	createInfo.viewType = vk::ImageViewType::e2D;
	createInfo.components.r = vk::ComponentSwizzle::eIdentity;
	createInfo.components.g = vk::ComponentSwizzle::eIdentity;
	createInfo.components.b = vk::ComponentSwizzle::eIdentity;
	createInfo.components.a = vk::ComponentSwizzle::eIdentity;
	createInfo.subresourceRange.aspectMask = aspect;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;
	
	
	return device.createImageView(createInfo);
}

vk::Format vkImage::find_supported_format(vk::PhysicalDevice physicalDevice, const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
	for (auto& format : candidates) {
		auto properties = physicalDevice.getFormatProperties(format);
		if (tiling == vk::ImageTiling::eLinear
			&& (properties.linearTilingFeatures & features) == features) {
			return format;
		}

		if (tiling == vk::ImageTiling::eOptimal
			&& (properties.optimalTilingFeatures & features) == features) {
			return format;
		}

#ifndef NDEBUG
		std::runtime_error err("Failed to find supported format");
#endif // !NDEBUG

	}
}
