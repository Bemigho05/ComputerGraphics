#pragma once
#include "config.h"

namespace vkUtil {
	std::vector<char> readFile(const std::string& filename, const bool debug) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (debug && !file.is_open()) { std::cerr << "Failed to load\"" << filename << "\"" << std::endl; }
		size_t filesize(static_cast<size_t>(file.tellg()));

		std::vector<char> buffer(filesize);
		file.seekg(0);
		file.read(buffer.data(), filesize);
		file.close();
		return buffer;
	}

	vk::ShaderModule createModule(const std::string& filename, const vk::Device& device, const bool debug) {
		std::vector<char> sourceCode = readFile(filename, debug);
		vk::ShaderModuleCreateInfo moduleInfo = {};
		moduleInfo.flags = vk::ShaderModuleCreateFlags();
		moduleInfo.codeSize = sourceCode.size();
		moduleInfo.pCode = reinterpret_cast<const uint32_t*>(sourceCode.data());
		try {
			return device.createShaderModule(moduleInfo);
		}
		catch(vk::SystemError err) {
			if (debug) { std::cerr << "Failed to create shader module for \"" << filename << "\"" << std::endl; }
		}
	}
}