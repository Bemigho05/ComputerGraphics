#pragma once
#include <vulkan/vulkan.hpp>
#include <memory>
#include <iostream>
#include <set>
#include <optional>
#include <fstream>
#include <sstream>
#include <queue>
#include <iomanip>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE


enum class meshTypes {
	GROUND,
	GIRL,
	SKULL
};

std::vector<std::string> split(std::string line, std::string delimiter);