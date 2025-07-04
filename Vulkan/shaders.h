#pragma once
#include "config.h"

namespace vkUtil {
	std::vector<char> readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

#ifndef NDEBUG
		if (!file.is_open()) { std::cerr << "Failed to load\"" << filename << "\"" << std::endl; }
#endif
		size_t filesize(static_cast<size_t>(file.tellg()));

		std::vector<char> buffer(filesize);
		file.seekg(0);
		file.read(buffer.data(), filesize);
		file.close();
		return buffer;
	}

	vk::ShaderModule createModule(const std::string& filename, const vk::Device& device) {
		std::vector<char> sourceCode = readFile(filename);
		vk::ShaderModuleCreateInfo moduleInfo = {};
		moduleInfo.flags = vk::ShaderModuleCreateFlags();
		moduleInfo.codeSize = sourceCode.size();
		moduleInfo.pCode = reinterpret_cast<const uint32_t*>(sourceCode.data());
		try {
			return device.createShaderModule(moduleInfo);
		}
		catch(vk::SystemError err) {

#ifndef NDEBUG
			std::cerr << "Failed to create shader module for \"" << filename << "\"" << std::endl;
#endif
		}
	}
}