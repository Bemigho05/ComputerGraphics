#pragma once
#include "config.h"



namespace vkMesh {
	std::vector<vk::VertexInputBindingDescription> getPosColorBindingDescription() {
		std::vector<vk::VertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = 7 * sizeof(float);
		bindingDescriptions[0].inputRate = vk::VertexInputRate::eVertex;

		return bindingDescriptions;
	}

	
	std::vector<vk::VertexInputAttributeDescription> getPosColorAttributeDescriptions() {

		std::vector<vk::VertexInputAttributeDescription> attributes(3);

		// Position
		attributes[0].binding = 0;
		attributes[0].location = 0;
		attributes[0].format = vk::Format::eR32G32Sfloat;
		attributes[0].offset = 0;


		// Color
		attributes[1].binding = 0;
		attributes[1].location = 1;
		attributes[1].format = vk::Format::eR32G32B32Sfloat;
		attributes[1].offset = 2 * sizeof(float);


		// texture
		attributes[2].binding = 0;
		attributes[2].location = 2;
		attributes[2].format = vk::Format::eR32G32Sfloat;
		attributes[2].offset = 5 * sizeof(float);

		return attributes;
	}
};