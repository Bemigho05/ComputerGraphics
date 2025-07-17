#pragma once
#include "config.h"

namespace vkMesh {
	std::vector<vk::VertexInputBindingDescription> getPosColorBindingDescriptions();
	std::vector<vk::VertexInputAttributeDescription> getPosColorAttributeDescriptions();
};