#include "frame.h"
#include "image.h"

void vkUtil::SwapChainFrame::create_descriptor_resources()
{
	BufferInput input;
	input.device = device;
	input.physicalDevice = physicalDevice;
	input.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	input.size = sizeof(UBO);
	input.usage = vk::BufferUsageFlagBits::eUniformBuffer;
	cameraDataBuffer = createBuffer(input);

	cameraDataWriteLocation = device.mapMemory(cameraDataBuffer.bufferMemory, 0, sizeof(UBO));

	input.size = 1024 * sizeof(glm::mat4);
	input.usage = vk::BufferUsageFlagBits::eStorageBuffer;
	modelTransformsBuffer = createBuffer(input);

	modelTransformsWriteLocation = device.mapMemory(modelTransformsBuffer.bufferMemory, 0, 1024 * sizeof(glm::mat4));

	modelTransforms.reserve(1024);

	for (int i = 0; i < 1024; ++i) {
		modelTransforms.push_back(glm::mat4(1.0f));
	}


	cameraDataBufferDescriptor.buffer = cameraDataBuffer.buffer;
	cameraDataBufferDescriptor.offset = 0;
	cameraDataBufferDescriptor.range = sizeof(UBO);

	modelTransformsBufferDescriptor.buffer = modelTransformsBuffer.buffer;
	modelTransformsBufferDescriptor.offset = 0;
	modelTransformsBufferDescriptor.range = 1024 * sizeof(glm::mat4);
}

void vkUtil::SwapChainFrame::create_depth_resources()
{
	depthFormat = vkImage::find_supported_format(
		physicalDevice, 
		{ vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint },
		vk::ImageTiling::eOptimal, 
		vk::FormatFeatureFlagBits::eDepthStencilAttachment
	);

	vkImage::ImageCreateInput imageInfo;
	imageInfo.device = device;
	imageInfo.physicalDevice = physicalDevice;
	imageInfo.tiling = vk::ImageTiling::eOptimal;
	imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
	imageInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
	imageInfo.width = width;
	imageInfo.height = height;
	imageInfo.format = depthFormat;
	depthBuffer = vkImage::create_image(imageInfo);
	depthBufferMemory = vkImage::create_image_memory(imageInfo, depthBuffer);
	depthBufferView = vkImage::create_image_view(device, depthBuffer, depthFormat, vk::ImageAspectFlagBits::eDepth);
}

void vkUtil::SwapChainFrame::write_descriptor_set()
{
	vk::WriteDescriptorSet writeInfoCameraData;

	writeInfoCameraData.dstSet = descriptorSet;
	writeInfoCameraData.dstBinding = 0;
	writeInfoCameraData.dstArrayElement = 0;
	writeInfoCameraData.descriptorCount = 1;
	writeInfoCameraData.descriptorType = vk::DescriptorType::eUniformBuffer;
	writeInfoCameraData.pBufferInfo = &cameraDataBufferDescriptor;

	device.updateDescriptorSets(writeInfoCameraData, nullptr);


	vk::WriteDescriptorSet writeInfoModelTransforms;

	writeInfoModelTransforms.dstSet = descriptorSet;
	writeInfoModelTransforms.dstBinding = 1;
	writeInfoModelTransforms.dstArrayElement = 0;
	writeInfoModelTransforms.descriptorCount = 1;
	writeInfoModelTransforms.descriptorType = vk::DescriptorType::eStorageBuffer;
	writeInfoModelTransforms.pBufferInfo = &modelTransformsBufferDescriptor;

	device.updateDescriptorSets(writeInfoModelTransforms, nullptr);


}

void vkUtil::SwapChainFrame::destroy() const
{
	device.destroyImage(depthBuffer);
	device.freeMemory(depthBufferMemory);
	device.destroyImageView(depthBufferView);


	device.waitIdle(); 
	device.destroySemaphore(imageAvailable);
	device.destroySemaphore(renderFinished);
	device.destroyFence(inFlight);
	device.destroyImageView(imageView);
	device.destroyFramebuffer(framebuffer);

	device.unmapMemory(cameraDataBuffer.bufferMemory);
	device.freeMemory(cameraDataBuffer.bufferMemory);
	device.destroyBuffer(cameraDataBuffer.buffer);

	device.unmapMemory(modelTransformsBuffer.bufferMemory);
	device.freeMemory(modelTransformsBuffer.bufferMemory);
	device.destroyBuffer(modelTransformsBuffer.buffer);
}
