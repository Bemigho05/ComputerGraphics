#pragma once
#include "config.h"

namespace vkInit {
	struct descriptorSetLayoutData {
		int count;
		std::vector<int> indices;
		std::vector<vk::DescriptorType> types;
		std::vector<int> counts;
		std::vector<vk::ShaderStageFlags> stages;
	};

	vk::DescriptorSetLayout create_descriptor_set_layout(vk::Device device, const descriptorSetLayoutData& bindings);

	vk::DescriptorPool create_descriptor_pool(vk::Device device, uint32_t size, const descriptorSetLayoutData& bindings);

	vk::DescriptorSet allocate_descriptor_set(vk::Device device, vk::DescriptorPool descriptorPool, vk::DescriptorSetLayout layout);
}