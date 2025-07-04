#pragma once
#include "config.h"
#include <stb_image.h>

namespace vkImage {
	struct TextureInput {
		vk::Device device;
		vk::PhysicalDevice physicalDevice;
		std::string filename;
		vk::CommandBuffer commandBuffer;
		vk::Queue queue;

		vk::DescriptorSetLayout layout;
		vk::DescriptorPool descriptorPool;

	};

	struct ImageCreateInput {
		vk::Device device;
		vk::PhysicalDevice physicalDevice;
		int width, height;
		vk::ImageUsageFlags usage;
		vk::MemoryPropertyFlags memoryProperties;
		vk::ImageTiling tiling;
		vk::Format format;
	};

	struct ImageLayoutTransitionInput {
		vk::CommandBuffer commandBuffer;
		vk::Queue queue;
		vk::Image image;
		vk::ImageLayout oldlayout, newlayout;
	};

	struct BufferImageCopyInput {
		vk::CommandBuffer commandBuffer;
		vk::Queue queue;
		vk::Buffer srcBuffer;
		vk::Image dstImage;
		int width, height;
	};


	class Texture {
	public:
		Texture(TextureInput info);

		void use(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout);

		~Texture();
	private:
		int width, height, channels;
		vk::Device device;
		vk::PhysicalDevice physicalDevice;
		std::string filename;
		stbi_uc* pixels;

	public:
		vk::Image image;
		vk::DeviceMemory imageMemory;
		vk::ImageView imageView;
		vk::Sampler sampler;

		vk::DescriptorSetLayout layout;
		vk::DescriptorSet descriptorSet;
		vk::DescriptorPool descriptorPool;

		vk::CommandBuffer commandBuffer;
		vk::Queue queue;

		void load();
		void populate();
		void create_view();
		void create_sampler();
		void create_descriptor_set();
	};

	vk::Image create_image(ImageCreateInput input);
	vk::DeviceMemory create_image_memory(ImageCreateInput input, vk::Image image);
	void transition_image_layout(ImageLayoutTransitionInput input);
	void copy_buffer_to_image(BufferImageCopyInput input);
	vk::ImageView create_image_view(vk::Device device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspect);
	vk::Format find_supported_format(vk::PhysicalDevice physicalDevice, const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
}