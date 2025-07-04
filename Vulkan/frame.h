#pragma once
#include "config.h"

#include "memory.h"

namespace vkUtil {
	struct UBO {
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 viewProjection;
	};

	class SwapChainFrame {
	public:
		vk::Device device;
		vk::PhysicalDevice physicalDevice;
		// swapchain
		vk::Image image;
		vk::ImageView imageView;
		vk::Framebuffer framebuffer;
		vk::Image depthBuffer;
		vk::DeviceMemory depthBufferMemory;
		vk::ImageView depthBufferView;
		vk::Format depthFormat;
		int width, height;

		vk::CommandBuffer commandBuffer;

		// synchronization
		vk::Semaphore imageAvailable, renderFinished;
		vk::Fence inFlight;

		// resources
		UBO cameraData;
		Buffer cameraDataBuffer;
		void* cameraDataWriteLocation;

		std::vector<glm::mat4> modelTransforms;
		Buffer modelTransformsBuffer;
		void* modelTransformsWriteLocation;


		vk::DescriptorBufferInfo cameraDataBufferDescriptor;
		vk::DescriptorBufferInfo modelTransformsBufferDescriptor;

		vk::DescriptorSet descriptorSet;
		

		void create_descriptor_resources();

		void create_depth_resources();

		void write_descriptor_set();

		void destroy() const;
	};
}