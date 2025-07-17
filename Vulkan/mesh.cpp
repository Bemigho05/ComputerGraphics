#include "mesh.h"

std::vector<vk::VertexInputBindingDescription> vkMesh::getPosColorBindingDescriptions()
{
	std::vector<vk::VertexInputBindingDescription> bindingDescriptions(1);
	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = 11 * sizeof(float);
	bindingDescriptions[0].inputRate = vk::VertexInputRate::eVertex;

	return bindingDescriptions;
}

std::vector<vk::VertexInputAttributeDescription> vkMesh::getPosColorAttributeDescriptions()
{

	std::vector<vk::VertexInputAttributeDescription> attributes(4);

	// Position
	attributes[0].binding = 0;
	attributes[0].location = 0;
	attributes[0].format = vk::Format::eR32G32B32Sfloat;
	attributes[0].offset = 0;


	// Color
	attributes[1].binding = 0;
	attributes[1].location = 1;
	attributes[1].format = vk::Format::eR32G32B32Sfloat;
	attributes[1].offset = 3 * sizeof(float);


	// texture
	attributes[2].binding = 0;
	attributes[2].location = 2;
	attributes[2].format = vk::Format::eR32G32Sfloat;
	attributes[2].offset = 6 * sizeof(float);

	// normal
	attributes[3].binding = 0;
	attributes[3].location = 3;
	attributes[3].format = vk::Format::eR32G32B32Sfloat;
	attributes[3].offset = 8 * sizeof(float);

	return attributes;
}
