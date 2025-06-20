#pragma once
#define _CRT_SECURE_NO_WARNINGS
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


enum class meshTypes {
	TRIANGLE,
	SQUARE,
	STAR
};